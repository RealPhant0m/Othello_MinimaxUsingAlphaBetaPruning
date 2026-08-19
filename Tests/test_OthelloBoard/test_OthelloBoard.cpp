#include "../../include/OthelloBoard.hpp"
#include <cassert>
#include <iostream>

using namespace othello;

void testInitialBoardState() {
    OthelloBoard board;
    auto boardString = board.convertBoardFormat();
    assert(boardString.size() == BOARD_SIZE * BOARD_SIZE);

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if ((row == 3 && col == 3) || (row == 4 && col == 4))
                assert(board.board()[row][col] == OthelloBoard::toChar(Player::White));
            else if ((row == 3 && col == 4) || (row == 4 && col == 3))
                assert(board.board()[row][col] == OthelloBoard::toChar(Player::Black));
            else
                assert(board.board()[row][col] == OthelloBoard::toChar(Player::Empty));
        }
    }
}

void testConvertBoardFormatRoundTrip() {
    std::string boardString(BOARD_SIZE * BOARD_SIZE, '0');
    boardString[3 * BOARD_SIZE + 3] = '1';
    boardString[3 * BOARD_SIZE + 4] = '2';
    boardString[4 * BOARD_SIZE + 3] = '2';
    boardString[4 * BOARD_SIZE + 4] = '1';

    OthelloBoard board(boardString);
    assert(board.convertBoardFormat() == boardString);
}

void testGetValidMovesEmptyBoard() {
    OthelloBoard board;
    auto validMoves = board.getValidMoves(Player::Black);
    assert(!validMoves.empty());
}

void testApplyMoveFlipsDiscs() {
    std::string boardString(BOARD_SIZE * BOARD_SIZE, '0');
    boardString[3 * BOARD_SIZE + 3] = '1';
    boardString[3 * BOARD_SIZE + 4] = '2';
    boardString[4 * BOARD_SIZE + 3] = '2';
    boardString[4 * BOARD_SIZE + 4] = '1';

    OthelloBoard board(boardString);
    auto validMoves = board.getValidMoves(Player::Black);
    assert(!validMoves.empty());

    bool foundMove = false;
    Move chosenMove;
    for (auto& move : validMoves) {
        if (move.row == 3 && move.col == 2) {
            foundMove = true;
            chosenMove = move;
            break;
        }
    }
    assert(foundMove);

    Board nextBoard = board.applyMove(chosenMove, Player::Black);
    assert(nextBoard[3][3] == OthelloBoard::toChar(Player::Black));
    assert(nextBoard[3][2] == OthelloBoard::toChar(Player::Black));
}

int main() {
    testInitialBoardState();
    testConvertBoardFormatRoundTrip();
    testGetValidMovesEmptyBoard();
    testApplyMoveFlipsDiscs();

    std::cout << "All OthelloBoard tests passed.\n";
    return 0;
}
