#include "OthelloBoard.hpp"

#include <stdexcept>

namespace othello {

OthelloBoard::OthelloBoard() {
    resetBoardToInitialPosition();
}

OthelloBoard::OthelloBoard(const Board& board) {
    board_ = board;
}

OthelloBoard::OthelloBoard(const std::string& boardString) {
    convertBoardFormat(boardString);
}

void OthelloBoard::convertBoardFormat(const std::string& boardString) {
    if (boardString.size() != static_cast<size_t>(BOARD_SIZE * BOARD_SIZE))
        throw std::invalid_argument("Board string must contain 64 characters.");
    for(int row = 0; row < BOARD_SIZE; row++)
        for(int col = 0; col < BOARD_SIZE; col++)
            board_[row][col] = boardString[(row * BOARD_SIZE) + col];
    return;
}

std::string OthelloBoard::convertBoardFormat() const {
    std::string stringBoard;
    stringBoard.reserve(BOARD_SIZE * BOARD_SIZE);
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int col = 0; col < BOARD_SIZE; col++)
            stringBoard.push_back(board_[row][col]);
    
    return stringBoard;
}

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

Board OthelloBoard::applyMove(const Move& move, Player player) const {
    Board nextBoard = board_;

    // Place the player's disc at the specified position.
    nextBoard[move.row][move.col] = toChar(player);

    auto flipDiscs = [&nextBoard, &player] (int row, int col, int directionIndex) {
        int r = row + directions[directionIndex].first;
        int c = col + directions[directionIndex].second;
        while (isInside(r, c) && nextBoard[r][c] == toChar(getOpponent(player))) {
            nextBoard[r][c] = toChar(player);
            r += directions[directionIndex].first;
            c += directions[directionIndex].second;
        }
    };

    for (int directionIndex : move.flippableDirections) {
        flipDiscs(move.row, move.col, directionIndex);
    }

    return nextBoard;
}

bool OthelloBoard::isOpponent(Player currentPlayer, int row, int col) const {
    return isInside(row, col) && 
           board_[row][col] != toChar(currentPlayer) && 
           board_[row][col] != toChar(Player::Empty);
}

Player OthelloBoard::getOpponent(Player currentPlayer) {
    return (currentPlayer == Player::White) ? Player::Black : Player::White;
}

const Board& OthelloBoard::board() const {
    return board_;
}

void OthelloBoard::resetBoardToInitialPosition() {
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            board_[row][col] = toChar(Player::Empty);
        }
    }
    board_[3][3] = toChar(Player::White);
    board_[3][4] = toChar(Player::Black);
    board_[4][3] = toChar(Player::Black);
    board_[4][4] = toChar(Player::White);
}

unsigned int OthelloBoard::countDiscsOnBoard() const {
    unsigned int discsCount = 0;
    for (int row = 0; row < BOARD_SIZE; row++)
        for (int col = 0; col < BOARD_SIZE; col++)
            if (board_[row][col] != toChar(Player::Empty))
                discsCount++;
    return discsCount;
}

bool OthelloBoard::isGameOver(std::vector<Move>& playerValidMoves, std::vector<Move>& opponentValidMoves) const {
    return (playerValidMoves.empty() && opponentValidMoves.empty());
}

void OthelloBoard::setCell(int row, int col, char value) {
    board_[row][col] = value;
}

bool OthelloBoard::isInside(int row, int col) {
    return (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
}

char OthelloBoard::toChar(Player player) {
    return static_cast<char>(player);
}

}