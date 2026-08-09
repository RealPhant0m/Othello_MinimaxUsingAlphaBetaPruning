#pragma once

#include <limits>
#include "OthelloBoard.hpp"

namespace othello {

// AI layer for choosing moves.
// This class is responsible for search and evaluation only.
// It depends on the board engine but does not manipulate the board directly in the UI layer.
class OthelloAI {
public:
    OthelloAI(Player player, unsigned int explorationDepth = 5);

    // Main public entry point.
    Move chooseMove(const OthelloBoard& board) const;

    // This controls whether the depth of the search is less than or 
    // greater than a certain limit. If more than about 10 or 11 depth is entered, 
    // the tree search will take a long time, so if a depth greater than 
    // the specified value is mistakenly entered, this method sets it to the specified maximum value.
    void setExplorationDepth(unsigned int depth = 5);

    // Optional statistics for debugging.
    unsigned long long getVisitedNodesCount() const;

private:
    // Define constants for positive and negative infinity to use in Minimax Algorithm.
    static constexpr float POSITIVE_INFINITY = std::numeric_limits<float>::max();
    static constexpr float NEGATIVE_INFINITY = -std::numeric_limits<float>::max();

    // To count the number of nodes visited in the minimax algorithm.
    mutable unsigned long long visitedNodesCount_ = 0;

    // The maximum depth that can be traversed in a Minimax tree.
    unsigned int explorationDepth_;

    Player player_;

    // Evaluation function
    float evaluate(const OthelloBoard& board, Player player, std::vector<Move>& playerValidMoves, std::vector<Move>& opponentValidMoves) const;

    // Heuristic Functions
    inline float calculateMobility(const OthelloBoard& board, Player player, std::vector<Move>& playerValidMoves, std::vector<Move>& opponentValidMoves) const;
    inline float calculateCoinParity(const OthelloBoard& board, Player player) const;
    inline float calculateStability(const OthelloBoard& board, Player player) const;
    inline float calculateCornerControl(const OthelloBoard& board, Player player) const;
    
};

} // namespace othello