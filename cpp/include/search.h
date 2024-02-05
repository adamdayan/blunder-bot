#ifndef SEARCH_H
#define SEARCH_H

#include <memory>
#include <vector>
#include <random>

#include "position.h"
#include "move_generator.h"
#include "net.h"

// TODO: have no idea what these numbers "should" be- run some experiments
constexpr int N_TO_CONSIDER = 16;
constexpr int N_SIMULATIONS = 200;

// hyperparameters taken from Danihelka, 2022
constexpr int C_VISIT = 50;
constexpr double C_SCALE = 1.0;

class GumbelMCTS;

struct Node {
  // TODO: find out if passing in MoveVec leads to meaningfully wasteful copying
  Node(float raw_prior, const Position pos, const Move move, bool is_root, MoveVec unexpanded_children)
      : raw_prior(raw_prior), pos(pos), move(move), is_root(is_root), unexpanded_children(unexpanded_children) {}

  // TODO: decide if the below is horrible and if there's a better way to do it
  // root node constructor, will leave Move empty because we've already taken move
  Node(const Position pos, MoveVec unexpanded_children)
      : raw_prior(0), pos(pos), move(Move()), is_root(true),
        unexpanded_children(unexpanded_children) {}

  float raw_prior;
  float applied_gumbel = 0; 
  float score;
  int visit_count = 0;
  float value = 0;
  const Move move;
  const Position pos;
  bool is_root = false;
  bool is_terminal = false;

  std::vector<std::unique_ptr<Node>> expanded_children;
  MoveVec unexpanded_children;
};

// executes Gumbel Monte Carlo Tree Search as described in "Policy Improvement
// By Planning With Gumbel" (Danihelka, 2022)
// https://openreview.net/pdf?id=bERaNdoegnO
class GumbelMCTS {
  public:
    GumbelMCTS(Net* net, int simulation_budget) : net(net), simulation_budget(simulation_budget) {}
    // executes Gumbel MCTS from a given position to find best move
    Move getBestMove(const Position& pos);

    // returns k nodes with highest prior + gumbel, called in argtop in Danihelka, 2022
    std::vector<Node*> getKGumbelArgtop(std::vector<Node*>& input_nodes, int k);

    // uses sequential halving to winnow the nodes to consider into 1 node
    // employing only a budget of n simulations
    Node* applySequentialHalving(Node* root, std::vector<Node*>& nodes_to_consider);

    // runs value head on child node and expands its children
    void expandAndEvaluate(Node* node);

    // called recursively to find unexpanded nodes and backpropagate their eval
    // up the tree. returns value of node
    int visit(Node* node);
  private:
    MoveGenerator move_gen;
    Net* net;
    int simulation_budget;
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::extreme_value_distribution<float> gumbel_dist{0.0};
};

#endif // SEARCH_H