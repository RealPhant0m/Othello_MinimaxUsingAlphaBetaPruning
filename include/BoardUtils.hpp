#pragma once

#include <iostream>
#include <string>
#include "OthelloBoard.hpp"

namespace othello {

class BoardUtils {
public:
    static std::string formatBoard(const OthelloBoard& board);
    static void printBoard(const OthelloBoard& board, std::ostream& out = std::cout);
    static std::pair<int, int> toCoordinates(int index);
    static int toIndex(int row, int col);
};

} // namespace othello
