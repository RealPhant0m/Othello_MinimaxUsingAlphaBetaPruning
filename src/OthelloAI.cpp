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

} // namespace othello
