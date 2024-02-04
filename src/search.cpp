#include <limits>
#include <memory>
#include <algorithm>
#include <unordered_set>

#include "search.h"
#include "move_generator.h"

Move GumbelMCTS::getBestMove(const Position& pos) {
  std::unique_ptr<Node> root = std::make_unique<Node>(pos, move_gen.generateMoves(pos));

}

int GumbelMCTS::select(Node* node) {
  std::vector<Node*> nodes_to_consider;
  for (const std::unique_ptr<Node>& child : node->expanded_children) {
    nodes_to_consider.push_back(child.get());
  }

  nodes_to_consider = getKGumbelArgtop(nodes_to_consider, std::min<int>(N_TO_CONSIDER, nodes_to_consider.size()));
  for (Node*& child : nodes_to_consider) {
    node->value += visit(child);
  }
}

// comparison func to sort nodes into descending order of processed_prior
bool nodeCompare(Node* lhs, Node* rhs) {
  return lhs->score > rhs->score;
}

std::vector<Node*> GumbelMCTS::getKGumbelArgtop(std::vector<Node*>& input_nodes, int k) {
  std::vector<Node*> nodes_to_consider;
  // add logit(move) + gumbel_variable
  for (Node* child : input_nodes) {
    // store the gumbel variable we applied to "avoid double-counting bias" (Danihelka, 2022)
    child->applied_gumbel = gumbel_dist(gen);
    child->score= child->raw_prior + child->applied_gumbel;
    nodes_to_consider.push_back(child);
  }
  
  // retain only the best k moves
  std::sort(nodes_to_consider.begin(), nodes_to_consider.end(), nodeCompare);
  nodes_to_consider.resize(k);
  return nodes_to_consider;
}

Node* GumbelMCTS::applySequentialHalving(std::vector<Node*>& nodes_to_consider, int n_simulations) {
  // IMPLEMENT!!!
}

void GumbelMCTS::expandAndEvaluate(Node* node) {
  // return if we've already expanded this node
  if (node->expanded_children.size() > 0) {
    return;
  }

  // expand node
  MoveVec legal_moves = move_gen.generateMoves(node->pos);
  std::unordered_set<Move> legal_move_set(legal_moves.begin(), legal_moves.end());
  std::vector<std::pair<Move, float>> moves_and_priors;
  // save the value head's evaluation of the position
  node->value = net->evaluatePosition(node->pos, moves_and_priors);
  // iterate through all moves suggested by net's policy head 
  for (const auto& move_prior : moves_and_priors) {
    // but only add nodes for the legal moves suggested by policy head
    if (legal_move_set.find(move_prior.first) != legal_move_set.end()) {
      Position new_pos = node->pos.applyMove(move_prior.first);
      node->expanded_children.emplace_back(
          std::make_unique<Node>(move_prior.second, new_pos, move_prior.first,
                                 false, move_gen.generateMoves(new_pos)));
    }
  }
}

int GumbelMCTS::visit(Node* node) {
  // if the node is unexpanded, expand it
  if (node->expanded_children.size() == 0) {
    expandAndEvaluate(node);
  } else {
    // otherwise find the best child and visit it
    node->visit_count++;
    Node* best_child = nullptr;
    int cur_highest_score = std::numeric_limits<float>::min();
    for (const auto& child : node->expanded_children) {
      // using both the value and ratio
      float score = child->value - child->visit_count / static_cast<float>(node->visit_count);
      if (score > cur_highest_score) {
        cur_highest_score = score;
        best_child = child.get();
      }
    }
    node->value += visit(best_child);
  }

  // NOTE: should this be negative value because side has switched? 
  node->visit_count++;
  return node->value;
}