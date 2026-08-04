#include "OthelloBoard.hpp"

namespace othello {

OthelloBoard::OthelloBoard() {}

std::vector<Move> OthelloBoard::getValidMoves(Player player) const {
    std::vector<Move> validMoves;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            std::vector<int> flippableDirections = getFlippableDirections(player, row, col);
            if (!flippableDirections.empty()) {
                Move move;
                move.row = row;
                move.col = col;
                move.flippableDirections = flippableDirections;
                validMoves.emplace_back(move);
            }
        }
    }
    return validMoves;
}

std::vector<int> OthelloBoard::getFlippableDirections(Player player, int row, int col) const {
    std::vector<int> validDirections;
    if (isInside(row, col) && board_[row][col] == toChar(Player::Empty)) {
        // check the 8 directions around the move for potential flips
        for (int i = 0; i < 8; i++) {
            int r = row + directions[i].first;
            int c = col + directions[i].second;
            bool foundOpponent = false;
            
            while (isOpponent(player, r, c)) {
                r += directions[i].first;
                c += directions[i].second;
                foundOpponent = true;
            }

            if (foundOpponent && isInside(r, c) && board_[r][c] == toChar(player)) {
                // Use validDirections vector to save dr AND dc.
                // Move is valid if it flips at least one opponent's disc.
                validDirections.push_back(i);
            }
        }
    }
    return validDirections;
}

bool OthelloBoard::isOpponent(Player player, int row, int col) const {
    return isInside(row, col) && 
           board_[row][col] != toChar(player) && 
           board_[row][col] != toChar(Player::Empty);
}

bool OthelloBoard::isInside(int row, int col) {
    return (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
}

char OthelloBoard::toChar(Player player) {
    return static_cast<char>(player);
}

}