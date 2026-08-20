// Test-only access to private OthelloAI members. Production files are unchanged.
#define private public
#include "../../include/OthelloAI.hpp"
#undef private

// OthelloAI's private inline helper definitions live in this .cpp file.
// Including it here makes those definitions available to these direct tests.
#include "../../src/OthelloAI.cpp"
#include "../../src/OthelloBoard.cpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace othello;

namespace {

constexpr float EPSILON = 0.001f;

void assertNear(float actual, float expected) {
    assert(std::fabs(actual - expected) < EPSILON);
}

OthelloBoard boardFromRows(const std::vector<std::string>& rows) {
    assert(rows.size() == BOARD_SIZE);
    std::string boardString;
    boardString.reserve(BOARD_SIZE * BOARD_SIZE);
    for (const auto& row : rows) {
        assert(row.size() == BOARD_SIZE);
        boardString += row;
    }
    return OthelloBoard(boardString);
}

int emptyStabilityMap(int map[BOARD_SIZE][BOARD_SIZE]) {
    for (int row = 0; row < BOARD_SIZE; ++row)
        for (int col = 0; col < BOARD_SIZE; ++col)
            map[row][col] = -2;
    return 0;
}

void testConstructionAndDepthLimits() {
    OthelloAI ai(Player::Black);
    assert(ai.explorationDepth_ == 6);

    ai.setExplorationDepth(0);
    assert(ai.explorationDepth_ == 0);

    ai.setExplorationDepth(11);
    assert(ai.explorationDepth_ == 11);

    ai.setExplorationDepth(12);
    assert(ai.explorationDepth_ == 6);
}

void testCornerControl() {
    OthelloAI ai(Player::Black);

    OthelloBoard initialBoard;
    assertNear(ai.calculateCornerControl(initialBoard, Player::Black), 0.0f);

    auto blackCorner = boardFromRows({
        "20000000", "00000000", "00000000", "00000000",
        "00000000", "00000000", "00000000", "00000000"
    });
    assertNear(ai.calculateCornerControl(blackCorner, Player::Black), 100.0f);
    assertNear(ai.calculateCornerControl(blackCorner, Player::White), -100.0f);

    auto oneCornerEach = boardFromRows({
        "20000001", "00000000", "00000000", "00000000",
        "00000000", "00000000", "00000000", "00000000"
    });
    assertNear(ai.calculateCornerControl(oneCornerEach, Player::Black), 0.0f);
}

void testPotentialCorners() {
    OthelloAI ai(Player::Black);

    auto blackCanReachTopLeft = boardFromRows({
        "01200000", "00000000", "00000000", "00000000",
        "00000000", "00000000", "00000000", "00000000"
    });
    assertNear(ai.calculatePotentialCorners(blackCanReachTopLeft, Player::Black), 100.0f);
    assertNear(ai.calculatePotentialCorners(blackCanReachTopLeft, Player::White), -100.0f);

    OthelloBoard initialBoard;
    float initialBoardScore = ai.calculatePotentialCorners(initialBoard, Player::Black);
    assert(std::isfinite(initialBoardScore));
    assert(initialBoardScore >= -100.0f && initialBoardScore <= 100.0f);
}

void testAxisStabilityEdgeAndFilledAxis() {
    OthelloAI ai(Player::Black);
    int map[BOARD_SIZE][BOARD_SIZE];
    emptyStabilityMap(map);

    auto edgeBoard = boardFromRows({
        "20000000", "00000000", "00000000", "00000000",
        "00000000", "00000000", "00000000", "00000000"
    });
    auto edgeStatus = ai.checkAxisStability(
        edgeBoard, map, 1, Player::Black, 0, 0, directions[2]);
    assert(edgeStatus == OthelloAI::Stable);

    auto filledBoard = boardFromRows({
        "22222222", "22222222", "22222222", "22222222",
        "22222222", "22222222", "22222222", "22222222"
    });
    emptyStabilityMap(map);
    auto filledStatus = ai.checkAxisStability(
        filledBoard, map, 1, Player::Black, 3, 3, directions[2]);
    assert(filledStatus == OthelloAI::Stable);
}

void testAxisStabilityKnownStableMapNeighbor() {
    OthelloAI ai(Player::Black);
    int map[BOARD_SIZE][BOARD_SIZE];
    emptyStabilityMap(map);
    map[3][2] = 1;

    OthelloBoard board = boardFromRows({
        "00000000", "00000000", "00000000", "00220000",
        "00000000", "00000000", "00000000", "00000000"
    });

    auto status = ai.checkAxisStability(
        board, map, 1, Player::Black, 3, 3, directions[2]);
    assert(status == OthelloAI::Stable);
}

void testDiscStability() {
    OthelloAI ai(Player::Black);
    int map[BOARD_SIZE][BOARD_SIZE];
    emptyStabilityMap(map);

    auto fullBoard = boardFromRows({
        "22222222", "22222222", "22222222", "22222222",
        "22222222", "22222222", "22222222", "22222222"
    });
    auto status = ai.checkDiscStability(fullBoard, map, 1, Player::Black, 3, 3);
    assert(status == OthelloAI::Stable);

    OthelloBoard initialBoard;
    emptyStabilityMap(map);
    auto emptyStatus = ai.checkDiscStability(
        initialBoard, map, 1, Player::Black, 1, 1);
    assert(emptyStatus == OthelloAI::SemiStable);
}

void testMobilityAndCoinParity() {
    OthelloAI ai(Player::Black);
    OthelloBoard initialBoard;
    std::vector<Move> playerMoves(3);
    std::vector<Move> opponentMoves(1);
    std::vector<Move> noMoves;

    assertNear(ai.calculateMobility(playerMoves, opponentMoves), 50.0f);
    assertNear(ai.calculateMobility(noMoves, opponentMoves), -100.0f);

    auto discs = boardFromRows({
        "20000000", "00000000", "00000000", "00000000",
        "00000000", "00000000", "00000000", "00000001"
    });
    assertNear(ai.calculateCoinParity(discs, Player::Black), 0.0f);
    assertNear(ai.calculateCoinParity(discs, Player::White), 0.0f);
    assertNear(ai.calculateCoinParity(initialBoard, Player::Black), 0.0f);
}

void testEvaluateinitialBoard() {
    OthelloAI ai(Player::Black);
    OthelloBoard board;
    std::vector<Move> playerMoves;
    std::vector<Move> opponentMoves;

    float score = ai.evaluate(board, Player::Black, playerMoves, opponentMoves);
    assert(std::isfinite(score));
    assert(score >= -100.0f && score <= 100.0f);
}

void testChooseMoveAndVisitedNodeCount() {
    OthelloAI ai(Player::Black, 1);
    auto initialPosition = boardFromRows({
        "00000000", "00000000", "00000000", "00012000",
        "00021000", "00000000", "00000000", "00000000"
    });
    Move move = ai.chooseMove(initialPosition);
    assert(move.row >= 0 && move.row < BOARD_SIZE);
    assert(move.col >= 0 && move.col < BOARD_SIZE);
    assert(!move.flippableDirections.empty());
    assert(ai.getVisitedNodesCount() > 0);

    ai.setExplorationDepth(0);
    initialPosition = boardFromRows({
        "00000000", "00000000", "00000000", "00212000",
        "00021000", "00000000", "00000000", "00000000"
    });
    move = ai.chooseMove(initialPosition);
    assert(move.row >= 0 && move.row < BOARD_SIZE);
    assert(move.col >= 0 && move.col < BOARD_SIZE);
    assert(!move.flippableDirections.empty());
    assert(ai.getVisitedNodesCount() > 0);
}

} // namespace

int main() {
    testConstructionAndDepthLimits();
    testCornerControl();
    testPotentialCorners();
    testAxisStabilityEdgeAndFilledAxis();
    testAxisStabilityKnownStableMapNeighbor();
    testDiscStability();
    testMobilityAndCoinParity();
    testEvaluateinitialBoard();
    testChooseMoveAndVisitedNodeCount();

    std::cout << "All OthelloAI tests passed.\n";
    return 0;
}
