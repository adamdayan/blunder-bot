#ifndef NET_H
#define NET_H

#include <vector> 

#include "position.h"

// NOTE: this interface will probably only be used until I have an actually trained and working net
// interface for 2-headed neural net
class Net {
  public:
    // returns output of value head by value and output of policy head by reference
    virtual int evaluatePosition(const Position& pos, std::vector<std::pair<Move, float>>& policy) = 0;
};

class DummyNet : Net {
  public:
    int evaluatePosition(const Position& pos, std::vector<std::pair<Move, float>>& policy) override;
};

#endif // NET_H