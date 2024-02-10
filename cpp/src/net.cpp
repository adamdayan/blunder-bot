#include <ATen/ops/ones.h>
#include <torch/nn/options/activation.h>
#include <torch/torch.h>
#include <torch/nn/functional.h>

#include "net.h"
#include "constants.h"
#include "utils.h"
#include "squares.h"

#include "stdio.h"

void printTensorShape(torch::Tensor& tensor) {
  printf("tensor shape: (");
  for (auto& size : tensor.sizes()) {
    printf(" %ld,", size);
  }
  printf(")\n");
}

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
  Position transformed_pos;
  // we have to flip input if black is to move
  if (side_to_move == Colour::Black) {
    transformed_pos = pos.flip();
  } else {
    transformed_pos = pos;
  }
  std::vector<torch::Tensor> to_move_tensors;
  std::vector<torch::Tensor> opponent_tensors;

  // get tensors for each piece type per player
  for (int piece = PieceType::Pawn; piece <= PieceType::King; piece++) {
    BitBoard to_move_bb = transformed_pos.getPieceBitBoard(Colour::White, static_cast<PieceType>(piece));
    to_move_tensors.push_back(bitboardToTensor(to_move_bb));

    BitBoard opponent_bb = transformed_pos.getPieceBitBoard(Colour::Black, static_cast<PieceType>(piece));
    opponent_tensors.push_back(bitboardToTensor(opponent_bb));
  }  
  // concatenate all the tensors
  torch::Tensor to_move_tensor = torch::cat(to_move_tensors, 0);
  torch::Tensor opponent_tensor = torch::cat(opponent_tensors, 0);
  return torch::cat({to_move_tensor, opponent_tensor}, 0);
}

torch::Tensor castlingRightsToTensor(const Position& pos) {
  Position transformed_pos;
  // we have to flip input if black is to move
  if (pos.getSideToMove() == Colour::Black) {
    transformed_pos = pos.flip();
  } else {
    transformed_pos = pos;
  }
  torch::Tensor castling_tensor;
  if (transformed_pos.canCastle(Colour::White, CastlingType::Kingside)) {
    castling_tensor = torch::ones({1, 8, 8});
  } else {
    castling_tensor = torch::zeros({1, 8, 8});
  }

  if (transformed_pos.canCastle(Colour::White, CastlingType::Queenside)) {
    castling_tensor = torch::cat({castling_tensor, torch::ones({1, 8, 8})}, 0);
  } else {
    castling_tensor = torch::cat({castling_tensor, torch::zeros({1, 8, 8})}, 0);
  }

  if (transformed_pos.canCastle(Colour::Black, CastlingType::Kingside)) {
    castling_tensor = torch::cat({castling_tensor, torch::ones({1, 8, 8})}, 0);
  } else {
    castling_tensor = torch::cat({castling_tensor, torch::zeros({1, 8, 8})}, 0);
  }

  if (transformed_pos.canCastle(Colour::Black, CastlingType::Queenside)) {
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
      parent = parent->getParent();
    } else {
      tensor = torch::cat({tensor, torch::zeros({12, 8, 8})}, 0);
    }
  }

  tensor = torch::cat({tensor, castlingRightsToTensor(pos)},0);

  // if it's white to move add a plane of 1s, if black add a plane of zeros
  // this is how neural net will "know" which colour is actually to move because we
  // transform the board repr so it's always white to move
  if (pos.getSideToMove() == Colour::White) {
    tensor = torch::cat({tensor, torch::ones({1, 8, 8})}, 0);
  } else {
    tensor = torch::cat({tensor, torch::zeros({1, 8, 8})}, 0);
  }

  // help BlunderNet find edge of board with a plane of just 1s
  tensor = torch::cat({tensor, torch::ones({1, 8, 8})}, 0);
  tensor = tensor.unsqueeze(0);

  return tensor;
}

// NOTE: not happy with this - return to it
void policyTensorToMoves(torch::Tensor& policy_tensor, std::vector<std::pair<Move, float>>& policy, const Position& pos) {
  int king_index = pos.getPieceBitBoard(pos.getSideToMove(), PieceType::King).getHighestSetBit();
  // normal moves + promotion moves
  for (int i = 0; i < ((64 * 64) + (2 * 8 * 8 * 4)); i++) {
    // extract move logit
    float score = policy_tensor[0][i].item().toFloat();
    Move move;
    // normal moves
    if (i < 64 * 64) {
      int source = i / 64;
      int dest = i % 64;
      // if black is to move we will have flipped the input so we must flip the
      // output
      if (pos.getSideToMove() == Colour::Black) {
        source = source ^ 0x38;
        dest = dest ^ 0x38;
      }
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
      } else { // TODO: we should never see this because of the pos flip so probably remove
        source = rebased_source - 8; 
        dest = rebased_dest;
      }

      if (pos.getSideToMove() == Colour::Black) {
        source = source ^ 0x38;
        dest = dest ^ 0x38;
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

double BlunderNet::getEvaluation(const Position& pos, std::vector<std::pair<Move, float>>& policy) {
  torch::Tensor input = inputToTensor(pos);

  auto output = net.forward({input});

  torch::Tensor policy_tensor = output.toTuple()->elements()[0].toTensor();
  policy_tensor = torch::nn::functional::softmax(
      policy_tensor, torch::nn::functional::SoftmaxFuncOptions(1));
  policyTensorToMoves(policy_tensor, policy, pos);

  double value = output.toTuple()->elements()[1].toDouble();
  // if we flipped board then we must also flip evaluation
  if (pos.getSideToMove() == Colour::Black) {
    value = value * -1;
  }
  return value;
}
