#include "OthelloAI.hpp"

#include <algorithm>

namespace othello {

OthelloAI::OthelloAI(Player player, unsigned int explorationDepth = 6) : player_(player) {
    setExplorationDepth(explorationDepth);
}

void OthelloAI::setExplorationDepth(unsigned int depth = 6) {
    if (depth >= 0 && depth <= 11)
        explorationDepth_ = depth;
    else
        explorationDepth_ = 6;
}

// This method returns the number of nodes visited 
// in the last next move calculation by the minimax algorithm.
unsigned long long OthelloAI::getVisitedNodesCount() const {
    return visitedNodesCount_;
}

float OthelloAI::evaluate(const OthelloBoard& board, Player player, std::vector<Move>& playerValidMoves, std::vector<Move>& opponentValidMoves) const {
    return (((weightOfCornerControl_    != 0 ) ? (weightOfCornerControl_    * calculateCornerControl(board, player))                                  : (0)) +
            ((weightOfPotentialCorners_ != 0 ) ? (weightOfPotentialCorners_ * calculatePotentialCorners(board, player))                               : (0)) +
            ((weightOfStability_        != 0 ) ? (weightOfStability_        * calculateStability(board, player))                                      : (0)) +
            ((weightOfMobility_         != 0 ) ? (weightOfMobility_         * calculateMobility(board, player, playerValidMoves, opponentValidMoves)) : (0)) +
            ((weightOfCoinParity_       != 0 ) ? (weightOfCoinParity_       * calculateCoinParity(board, player))                                     : (0)));
}

inline float OthelloAI::calculateCornerControl(const OthelloBoard& board, Player player) const {
    const auto& boardFormat = board.board();
    int playerCornersCount = 0;
    int opponentCornersCount = 0;
    const std::array<std::pair<int, int>, 4> corners = {{{0, 0}, {0, 7}, {7, 0}, {7, 7}}};
    for (const auto& [row, col] : corners) {
        const char cell = boardFormat[row][col];
        if (cell == OthelloBoard::toChar(player))
            ++playerCornersCount;
        else if (cell == OthelloBoard::toChar(OthelloBoard::getOpponent(player)))
            ++opponentCornersCount;
    }
    if (playerCornersCount + opponentCornersCount != 0)
        return ((static_cast<float>(playerCornersCount - opponentCornersCount) / static_cast<float>(playerCornersCount + opponentCornersCount)) * 100);
    return 0;
}

inline float OthelloAI::calculatePotentialCorners(const OthelloBoard& board, Player player) const {
    int playerPotentialCornersCount = 0;
    int opponentPotentialCornersCount = 0;
    int dr; // Delta Row
    int dc; // Delta Column
    const auto& boardFormat = board.board();

    auto checkPotentialCorner = [&boardFormat, &player, &playerPotentialCornersCount, &opponentPotentialCornersCount] 
                                (int dr_, int dc_, int directionIndex) {
                            
        if (boardFormat[dr_][dc_] == OthelloBoard::toChar(OthelloBoard::getOpponent(player))) {
            while (OthelloBoard::isInside(dr_, dc_) && boardFormat[dr_][dc_] == OthelloBoard::toChar(OthelloBoard::getOpponent(player))) {
                dr_ += directions[directionIndex].first;
                dc_ += directions[directionIndex].second;
            }
            if (OthelloBoard::isInside(dr_, dc_) && boardFormat[dr_][dc_] == OthelloBoard::toChar(player))
                playerPotentialCornersCount++;
        } else if (boardFormat[dr_][dc_] == OthelloBoard::toChar(player)) {
            while (OthelloBoard::isInside(dr_, dc_) && boardFormat[dr_][dc_] == OthelloBoard::toChar(player)) {
                dr_ += directions[directionIndex].first;
                dc_ += directions[directionIndex].second;
            }
            if (OthelloBoard::isInside(dr_, dc_) && boardFormat[dr_][dc_] == OthelloBoard::toChar(OthelloBoard::getOpponent(player)))
                opponentPotentialCornersCount++;
        }
    };

    // Check top-left corner
    if (boardFormat[0][0] == OthelloBoard::toChar(Player::Empty)) {
        dr = directions[2].first;
        dc = directions[2].second;
        checkPotentialCorner(dr, dc, 2);
        
        dr = directions[3].first;
        dc = directions[3].second;
        checkPotentialCorner(dr, dc, 3);
        
        dr = directions[4].first;
        dc = directions[4].second;
        checkPotentialCorner(dr, dc, 4);
    }

    // Check top-right corner
    if (boardFormat[0][BOARD_SIZE - 1] == OthelloBoard::toChar(Player::Empty)) {
        dr = directions[6].first;
        dc = BOARD_SIZE - 1 + directions[6].second;
        checkPotentialCorner(dr, dc, 6);

        dr = directions[5].first;
        dc = BOARD_SIZE - 1 + directions[5].second;
        checkPotentialCorner(dr, dc, 5);
        
        dr = directions[4].first;
        dc = BOARD_SIZE - 1 + directions[4].second;
        checkPotentialCorner(dr, dc, 4);
    }

    // Check bottom-left corner
    if (boardFormat[BOARD_SIZE - 1][0] == OthelloBoard::toChar(Player::Empty)) {
        dr = BOARD_SIZE - 1 + directions[2].first;
        dc = directions[2].second;
        checkPotentialCorner(dr, dc, 2);

        dr = BOARD_SIZE - 1 + directions[1].first;
        dc = directions[1].second;
        checkPotentialCorner(dr, dc, 1);

        dr = BOARD_SIZE - 1 + directions[0].first;
        dc = directions[0].second;
        checkPotentialCorner(dr, dc, 0);
    }

    // Check bottom-right corner
    if (boardFormat[BOARD_SIZE - 1][BOARD_SIZE - 1] == OthelloBoard::toChar(Player::Empty)) {
        dr = BOARD_SIZE - 1 + directions[0].first;
        dc = BOARD_SIZE - 1 + directions[0].second;
        checkPotentialCorner(dr, dc, 0);

        dr = BOARD_SIZE - 1 + directions[7].first;
        dc = BOARD_SIZE - 1 + directions[7].second;
        checkPotentialCorner(dr, dc, 7);

        dr = BOARD_SIZE - 1 + directions[6].first;
        dc = BOARD_SIZE - 1 + directions[6].second;
        checkPotentialCorner(dr, dc, 6);
    }

    if (playerPotentialCornersCount + opponentPotentialCornersCount != 0)
        return ((static_cast<float>(playerPotentialCornersCount - opponentPotentialCornersCount) / static_cast<float>(playerPotentialCornersCount + opponentPotentialCornersCount)) * 100);
    return 0;
}

inline float OthelloAI::calculateMobility(const OthelloBoard& board, Player player, std::vector<Move>& playerValidMoves, std::vector<Move>& opponentValidMoves) const {
    int playerValidMovesCount = playerValidMoves.size();
    int opponentValidMovesCount = opponentValidMoves.size();
    if (playerValidMovesCount + opponentValidMovesCount != 0)
        return ((static_cast<float>(playerValidMovesCount - opponentValidMovesCount) / static_cast<float>(playerValidMovesCount + opponentValidMovesCount)) * 100);
    return 0;
}

inline float OthelloAI::calculateCoinParity(const OthelloBoard& board, Player player) const {
    int playerDiscsCount = 0;
    int opponentDiscsCount = 0;
    const auto& boardFormat = board.board();
    for (const auto& row : boardFormat) {
        for (char cell : row) {
            if (cell == OthelloBoard::toChar(player))
                playerDiscsCount++;
            else if (cell != OthelloBoard::toChar(Player::Empty))
                opponentDiscsCount++;
        }
    }
    if (playerDiscsCount + opponentDiscsCount != 0)
        return ((static_cast<float>(playerDiscsCount - opponentDiscsCount) / static_cast<float>(playerDiscsCount + opponentDiscsCount)) * 100);
    return 0;
}

} // namespace othello
