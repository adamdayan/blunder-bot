# Early Jan 2024
Implemented bitboards, move generation and wrote a whole bunch of tests

# Late Jan - 09/02/2024
Had a first go at Gumbel MCTS - not sure I've got it right as it's hard to test without a working neural net to provide move probs and evaluation

# 10/02/2024
Trying to train a neural net on the Lichess Elite database of masters games - this a) is just to bootstrap my model to reduce the amount of RL that needs to be done and b) is very likely not the final architecture but rather just an initial network to allow testing of the MCTS implementation. Multi-process dataloads really speed things up btw!