#include "../../include/BoardUtils.hpp"
#include "../../include/OthelloBoard.hpp"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

using namespace othello;

void testFormatBoardEmptyBoard() {
    OthelloBoard board;
    std::string result = BoardUtils::formatBoard(board);

    assert(!result.empty());
    assert(result.find("0") != std::string::npos);
    assert(result.back() == '\n');
}

void testFormatBoardWithCustomState() {
    std::string boardString(BOARD_SIZE * BOARD_SIZE, '0');
    boardString[0] = '1';
    boardString[1] = '2';
    boardString[BOARD_SIZE + 0] = '2';
    boardString[BOARD_SIZE + 1] = '1';

    OthelloBoard board(boardString);
    std::string formatted = BoardUtils::formatBoard(board);

    assert(formatted.find("1 2") != std::string::npos);
    assert(formatted.find("2 1") != std::string::npos);
}

void testPrintBoardUsesProvidedStream() {
    OthelloBoard board;
    std::ostringstream out;

    BoardUtils::printBoard(board, out);

    std::string printed = out.str();
    assert(!printed.empty());
    assert(printed == BoardUtils::formatBoard(board));
}

void testToCoordinatesForFirstAndLastCells() {
    auto [row0, col0] = BoardUtils::toCoordinates(0);
    auto [rowLast, colLast] = BoardUtils::toCoordinates(BOARD_SIZE * BOARD_SIZE - 1);

    assert(row0 == 0 && col0 == 0);
    assert(rowLast == BOARD_SIZE - 1 && colLast == BOARD_SIZE - 1);
}

void testToCoordinatesForMiddleIndex() {
    auto [row, col] = BoardUtils::toCoordinates(BOARD_SIZE + 3);
    assert(row == 1 && col == 3);
}

void testToIndexForBoundaryValues() {
    assert(BoardUtils::toIndex(0, 0) == 0);
    assert(BoardUtils::toIndex(0, BOARD_SIZE - 1) == BOARD_SIZE - 1);
    assert(BoardUtils::toIndex(BOARD_SIZE - 1, 0) == (BOARD_SIZE - 1) * BOARD_SIZE);
    assert(BoardUtils::toIndex(BOARD_SIZE - 1, BOARD_SIZE - 1) == BOARD_SIZE * BOARD_SIZE - 1);
}

void testRoundTripIndexConversion() {
    for (int index = 0; index < BOARD_SIZE * BOARD_SIZE; ++index) {
        auto [row, col] = BoardUtils::toCoordinates(index);
        assert(BoardUtils::toIndex(row, col) == index);
    }
}

int main() {
    testFormatBoardEmptyBoard();
    testFormatBoardWithCustomState();
    testPrintBoardUsesProvidedStream();
    testToCoordinatesForFirstAndLastCells();
    testToCoordinatesForMiddleIndex();
    testToIndexForBoundaryValues();
    testRoundTripIndexConversion();

    std::cout << "All BoardUtils tests passed.\n";
    return 0;
}
