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
    return (((weightOfCornerControl_ != 0 ) ? (weightOfCornerControl_ * calculateCornerControl(board, player))                                  : (0)) +
            ((weightOfStability_     != 0 ) ? (weightOfStability_     * calculateStability(board, player))                                      : (0)) +
            ((weightOfMobility_      != 0 ) ? (weightOfMobility_      * calculateMobility(board, player, playerValidMoves, opponentValidMoves)) : (0)) +
            ((weightOfCoinParity_    != 0 ) ? (weightOfCoinParity_    * calculateCoinParity(board, player))                                     : (0)));
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
