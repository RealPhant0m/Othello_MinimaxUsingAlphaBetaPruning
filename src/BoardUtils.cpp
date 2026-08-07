#include "BoardUtils.hpp"

namespace othello {

std::string BoardUtils::formatBoard(const OthelloBoard& board) {
    std::string output;
    const auto& b = board.board();
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            output += b[row][col];
            output += ' ';
        }
        output += '\n';
    }
    return output;
}

void BoardUtils::printBoard(const OthelloBoard& board, std::ostream& out) {
    out << formatBoard(board);
}

std::pair<int, int> BoardUtils::toCoordinates(int index) {
    return {index / BOARD_SIZE, index % BOARD_SIZE};
}

int BoardUtils::toIndex(int row, int col) {
    return (row * BOARD_SIZE + col);
}

} // namespace othello
