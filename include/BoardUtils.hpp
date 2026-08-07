#pragma once

#include <iostream>
#include <string>
#include "OthelloBoard.hpp"

namespace othello {

// Utility layer for presentation and conversion.
// This class helps print the board and convert between index and coordinates.
class BoardUtils {
public:
    static std::string formatBoard(const OthelloBoard& board);
    static void printBoard(const OthelloBoard& board, std::ostream& out = std::cout);
    static std::pair<int, int> toCoordinates(int index);
    static int toIndex(int row, int col);
};

} // namespace othello
