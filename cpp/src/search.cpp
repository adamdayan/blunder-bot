#include <limits>
#include <memory>
#include <algorithm>
#include <unordered_set>

#include "search.h"
#include "move_generator.h"

Move GumbelMCTS::getBestMove(const Position& pos) {
  std::unique_ptr<Node> root = std::make_unique<Node>(pos, move_gen.generateMoves(pos));
  expandAndEvaluate(root.get());  

  std::vector<Node*> nodes_to_consider;
  for (const std::unique_ptr<Node>& child : root->expanded_children) {
    nodes_to_consider.push_back(child.get());
  }

  nodes_to_consider = getKGumbelArgtop(nodes_to_consider, std::min<int>(N_TO_CONSIDER, nodes_to_consider.size()));
  for (Node*& child : nodes_to_consider) {
    // NOTE: should this be negative?
    root->value += -visit(child);
  }

  // NOTE: when we are training with self-play we will need to save the
  // completed Q-values somewhere
  Node* best_move = applySequentialHalving(root.get(), nodes_to_consider);
  return best_move->move;
}

// comparison func to sort nodes into descending order of processed_prior
bool nodeCompare(Node* lhs, Node* rhs) {
  return lhs->score > rhs->score;
}

std::vector<Node*> GumbelMCTS::getKGumbelArgtop(std::vector<Node*>& input_nodes, int k) {
  std::vector<Node*> nodes_to_consider;
  // add logit(move) + gumbel_variable
  printf("getKGumbelArgtop\n");
  for (Node* child : input_nodes) {
    // store the gumbel variable we applied to "avoid double-counting bias" (Danihelka, 2022)
    child->applied_gumbel = 0.5; // gumbel_dist(gen);  TODO: REVERT!!!
    child->score = child->raw_prior + child->applied_gumbel;
    nodes_to_consider.push_back(child);

    printf("move: %s raw_prior: %f gumbel: %f value: %f score: %f\n",
            child->move.to_string(*input_nodes[0]->pos.getParent(), true).c_str(), child->raw_prior,
            child->applied_gumbel, child->value, child->score);
  }
  
  // retain only the best k moves
  std::sort(nodes_to_consider.begin(), nodes_to_consider.end(), nodeCompare);
  nodes_to_consider.resize(k);
  return nodes_to_consider;
}

Node* GumbelMCTS::applySequentialHalving(Node* root, std::vector<Node*>& nodes_to_consider) {
  int n_simulations = simulation_budget;
  // repeatedly halve the number of nodes we're considering until we only have 1
  // remaining
  while (nodes_to_consider.size() > 1) {
    printf("\n\nn_simulations remaining: %d\n", n_simulations);
    // number of times to visit each node under consideration
    int n_visits_per_node = n_simulations / (std::log(nodes_to_consider.size()) * nodes_to_consider.size());
    int max_visit_cnt = std::numeric_limits<int>::min();
    for (Node* child : nodes_to_consider) {
      for (int i = 0; i < n_visits_per_node; i++) {
        root->value += -visit(child);
        n_simulations--;
      }
      max_visit_cnt = std::max(max_visit_cnt, child->visit_count);
    }

    // NOTE: unsure if this is needed
    // if we have left over simulations on the final iteration due to rounding,
    // spend the remaining budget
    if (nodes_to_consider.size() == 2 || nodes_to_consider.size() == 3 && n_simulations > 0) {
        int remaining_visits = n_simulations / nodes_to_consider.size();
        for (Node* child : nodes_to_consider) {
          for (int i = 0; i < remaining_visits; i++) {
            root->value += -visit(child);
          }
        }
    }

    for (Node* child : nodes_to_consider) {
      // calculate σ(ˆq(a))
      double sigma_qhat = (C_VISIT + max_visit_cnt) * (C_SCALE * -child->value);
      child->score = child->raw_prior + child->applied_gumbel + sigma_qhat;
      printf("move: %s raw_prior: %f gumbel: %f value: %f score: %f\n",
             child->move.to_string(root->pos, true).c_str(), child->raw_prior,
             child->applied_gumbel, child->value, child->score);
    }

    // remove the worst half
    std::sort(nodes_to_consider.begin(), nodes_to_consider.end(), nodeCompare);
    nodes_to_consider.resize(static_cast<int>(nodes_to_consider.size() / 2));
  }

  return nodes_to_consider[0];
}

void GumbelMCTS::expandAndEvaluate(Node* node) {
  // return if we've already expanded this node
  if (node->expanded_children.size() > 0) {
    return;
  }

  // expand node
  MoveVec legal_moves = move_gen.generateMoves(node->pos);

  // if we have no legal moves we must be checkmated or stalemated
  if (legal_moves.size() == 0) {
    // if not legal moves and king is in check then we have checkmate
    if (move_gen.isCheck(node->pos)) {
      node->value = -1;
    } else {
      // otherwise must be stalemate
      node->value = 0;
    }
    node->is_terminal = true;
    return;
  }

  // if it's a draw
  if (node->pos.isDraw()) {
    node->is_terminal = true;
    node->value = 0;
    return;
  }

  // otherwise evaluate position and add nodes for policy head's suggested moves
  std::unordered_set<Move> legal_move_set(legal_moves.begin(), legal_moves.end());
  std::vector<std::pair<Move, float>> moves_and_priors;
  // save the value head's evaluation of the position
  node->value = net->getEvaluation(node->pos, moves_and_priors);
  float legal_priors_total = 0;
  // iterate through all moves suggested by net's policy head 
  for (const auto& move_prior : moves_and_priors) {
    // but only add nodes for the legal moves suggested by policy head
    if (legal_move_set.find(move_prior.first) != legal_move_set.end()) {
      legal_priors_total += move_prior.second;
      Position new_pos = node->pos.applyMove(move_prior.first);
      node->expanded_children.emplace_back(
          std::make_unique<Node>(move_prior.second, new_pos, move_prior.first,
                                 false, move_gen.generateMoves(new_pos)));
    }
  }

  // renormalise probabilities using only legal moves
  for (const auto& child: node->expanded_children) {
    child->raw_prior /= legal_priors_total;
  }
}

// NOTE: when I make this multi-threaded I suspect this would be the place to start
int GumbelMCTS::visit(Node* node) {
  // if the node is unexpanded, expand it
  if (node->expanded_children.size() == 0) {
    expandAndEvaluate(node);
  } else {
    // otherwise find the best child and visit it
    node->visit_count++;
    Node* best_child = nullptr;
    float cur_highest_score = -std::numeric_limits<float>::max();
    for (const auto& child : node->expanded_children) {
      // using both the prior and ratio
      // NOTE: very unsure whether I should be using child->raw_prior or child->value here
      float score = child->raw_prior - child->visit_count / static_cast<float>(node->visit_count);
      if (score > cur_highest_score) {
        cur_highest_score = score;
        best_child = child.get();
      }
    }
    // TODO: double check this should be negative
    node->value += -visit(best_child);
  }

  node->visit_count++;
  return node->value;
}