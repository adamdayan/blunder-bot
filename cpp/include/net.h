#ifndef NET_H
#define NET_H

#include <vector> 
#include <string>
#include <torch/script.h>

#include "position.h"

// NOTE: this interface will probably only be used until I have an actually trained and working net
// interface for 2-headed neural net
class Net {
  public:
    // returns output of value head by value and output of policy head by reference
    virtual double getEvaluation(const Position& pos, std::vector<std::pair<Move, float>>& policy) = 0;
};

class DummyNet : public Net {
  public:
    double getEvaluation(const Position& pos, std::vector<std::pair<Move, float>>& policy) override;
};

class BlunderNet : public Net {
  public:
    BlunderNet(const std::string& model_path);
    double getEvaluation(const Position& pos, std::vector<std::pair<Move, float>>& policy) override;
  private:
    torch::jit::script::Module net;
};

#endif // NET_H