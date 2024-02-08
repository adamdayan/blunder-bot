#include <ATen/ops/ones.h>
#include <torch/nn/options/activation.h>
#include <torch/torch.h>
#include <torch/nn/functional.h>

#include "net.h"
#include "constants.h"
#include "utils.h"

#include "stdio.h"

// NOTE: should this perhaps be methods on bitboard and position?
torch::Tensor bitboardToTensor(BitBoard bb) {
  torch::Tensor board_tensor = torch::zeros({1, 8, 8});
  // set every board position where there is a piece
  while (!bb.isEmpty()) {
    int square_index = bb.popHighestSetBit();
    board_tensor[0][square_index / 8][square_index % 8] = 1;
  }

  return board_tensor;
}

// TODO: I suspect this is horribly innefficient - return to this!
torch::Tensor positionToTensor(const Position& pos, Colour side_to_move) {
  std::vector<torch::Tensor> to_move_tensors;
  std::vector<torch::Tensor> opponent_tensors;

  // get tensors for each piece type per player
  for (int piece = PieceType::Pawn; piece <= PieceType::King; piece++) {
    BitBoard to_move_bb = pos.getPieceBitBoard(side_to_move, static_cast<PieceType>(piece));
    to_move_tensors.push_back(bitboardToTensor(to_move_bb));

    BitBoard opponent_bb = pos.getPieceBitBoard(invertColour(side_to_move), static_cast<PieceType>(piece));
    opponent_tensors.push_back(bitboardToTensor(opponent_bb));
  }  
  // concatenate all the tensors
  torch::Tensor to_move_tensor = torch::cat(to_move_tensors, 0);
  torch::Tensor opponent_tensor = torch::cat(opponent_tensors, 0);
  return torch::cat({to_move_tensor, opponent_tensor}, 0);
}

torch::Tensor castlingRightsToTensor(const Position& pos) {
  torch::Tensor castling_tensor;
  if (pos.canCastle(pos.getSideToMove(), CastlingType::Kingside)) {
    castling_tensor = torch::ones({1, 8, 8});
  } else {
    castling_tensor = torch::zeros({1, 8, 8});
  }

  if (pos.canCastle(pos.getSideToMove(), CastlingType::Queenside)) {
    castling_tensor = torch::cat({castling_tensor, torch::ones({1, 8, 8})}, 0);
  } else {
    castling_tensor = torch::cat({castling_tensor, torch::zeros({1, 8, 8})}, 0);
  }

  if (pos.canCastle(invertColour(pos.getSideToMove()), CastlingType::Kingside)) {
    castling_tensor = torch::cat({castling_tensor, torch::ones({1, 8, 8})}, 0);
  } else {
    castling_tensor = torch::cat({castling_tensor, torch::zeros({1, 8, 8})}, 0);
  }

  if (pos.canCastle(invertColour(pos.getSideToMove()), CastlingType::Queenside)) {
    castling_tensor = torch::cat({castling_tensor, torch::ones({1, 8, 8})}, 0);
  } else {
    castling_tensor = torch::cat({castling_tensor, torch::zeros({1, 8, 8})}, 0);
  }

  return castling_tensor;
}

torch::Tensor inputToTensor(const Position& pos) {
  // add current board position
  torch::Tensor tensor = positionToTensor(pos, pos.getSideToMove());
  const Position* parent = pos.getParent();
  // add 5 previous positions
  for (int i = 0; i < POS_HISTORY_LEN; i++) {
    if (parent != nullptr) {
      tensor = torch::cat({tensor, positionToTensor(*parent, pos.getSideToMove())}, 0);
    } else {
      tensor = torch::cat({tensor, torch::zeros({12, 8, 8})}, 0);
    }
  }

  printf("tensor_dims after pos history %ld %ld %ld", tensor.dim(), tensor[0].dim(), tensor[0][0].dim());

  tensor = torch::cat({tensor, castlingRightsToTensor(pos)},0);

  // if it's white to move add a plane of 1s, if black add a plane of zeros
  if (pos.getSideToMove() == Colour::White) {
    tensor = torch::cat({tensor, torch::ones({1, 8, 8})}, 0);
  } else {
    tensor = torch::cat({tensor, torch::zeros({1, 8, 8})}, 0);
  }

  tensor = torch::cat({tensor, torch::ones({1, 8, 8})}, 0);

  return tensor;
}

// NOTE: not happy with this - return to it
void policyTensorToMoves(torch::Tensor& policy_tensor, std::vector<std::pair<Move, float>>& policy, const Position& pos) {
  int king_index = pos.getPieceBitBoard(pos.getSideToMove(), PieceType::King).getHighestSetBit();
  // normal moves + promotion moves
  for (int i = 0; i < ((64 * 64) + (2 * 8 * 8 * 4)); i++) {
    float score = policy_tensor[i].item().toFloat();
    Move move;
    // normal moves
    if (i < 64 * 64) {
      int source = i / 64;
      int dest = i % 64;
      // try to detect castling
      // NOTE: not very confident in this
      if (source == king_index && abs(source - dest) > 2) {
        if ((source == 4 && dest == 6) || (source == 60 && dest == 62)) {
          move = Move(source, dest, MoveType::KingsideCastle);
        } else if ((source == 4 && dest == 2) || (source == 60 && dest == 58)) {
          move = Move(source, dest, MoveType::QueensideCastle);
        }
      // TODO: this will not detect enpassant!!!
      } else if (pos.isOccupied(dest)) {
        move = Move(source, dest, MoveType::Capture);
      } else {
        move = Move(source, dest, MoveType::Quiet);
      }

    } else {
      // promotions
      int side = i / (8 * 8 * 4);
      int rebased_source = (i - side) / (8 * 4);
      int rebased_dest = (i - side - rebased_source) / 4;
      int piece = i - rebased_source  - rebased_dest;
      int source; 
      int dest;
      if (side == 0) {
        source = rebased_source + 48; 
        dest = rebased_dest + 57;
      } else {
        source = rebased_source - 8; 
        dest = rebased_dest;
      }
      if (pos.isOccupied(dest)) {
        move = Move(source, dest, MoveType::Capture, static_cast<PieceType>(piece + 1));
      } else {
        move = Move(source, dest, MoveType::Quiet, static_cast<PieceType>(piece + 1));
      }

    }
    policy.push_back(std::make_pair(move, score));
  }
}

BlunderNet::BlunderNet(const std::string& model_path) {
  net = torch::jit::load(model_path);
}

float BlunderNet::getEvaluation(const Position& pos, std::vector<std::pair<Move, float>>& policy) {
  torch::Tensor input = inputToTensor(pos);

  auto output = net.forward({input});

  torch::Tensor policy_tensor = output.toTuple()->elements()[0].toTensor();
  policy_tensor = torch::nn::functional::softmax(
      policy_tensor, torch::nn::functional::SoftmaxFuncOptions(0));
  policyTensorToMoves(policy_tensor, policy, pos);

  torch::Tensor value_tensor = output.toTuple()->elements()[1].toTensor();
  return value_tensor[0].item().toFloat();
}
