#include "net.h"
#include "stdio.h"

#include "position.h"
#include "constants.h"
#include "useful_fens.h"
#include "move_generator.h"
#include "zobrist_hash.h"
#include <search.h>
#include <net.h>


int main() {
  ZobristHash::initialiseKeys();

  Position pos;
  BlunderNet net("/home/adam/dev/blunder-bot/python/scripted_supervised_learning_model.pt");
  
  GumbelMCTS searcher(&net, 10);

  Move best_move = searcher.getBestMove(pos);

  return 0;
}