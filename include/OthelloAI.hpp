#pragma once

#include <limits>
#include "OthelloBoard.hpp"

namespace othello {

// AI layer for choosing moves.
// This class is responsible for search and evaluation only.
// It depends on the board engine but does not manipulate the board directly in the UI layer.
class OthelloAI {
public:
    OthelloAI(Player player, unsigned int explorationDepth = 6);

    // Main public entry point.
    Move chooseMove(const OthelloBoard& board);

    // This controls whether the depth of the search is less than or 
    // greater than a certain limit. If more than about 10 or 11 depth is entered, 
    // the tree search will take a long time, so if a depth greater than 
    // the specified value is mistakenly entered, this method sets it to the specified maximum value.
    void setExplorationDepth(int depth = 6);

    // Optional statistics for debugging.
    unsigned long long getVisitedNodesCount() const;

private:
    // Define constants for positive and negative infinity to use in Minimax Algorithm.
    static constexpr float POSITIVE_INFINITY = std::numeric_limits<float>::max();
    static constexpr float NEGATIVE_INFINITY = -std::numeric_limits<float>::max();

    enum DiscStabilityState {
        Stable,
        SemiStable,
        Unstable
    };

    // To count the number of nodes visited in the minimax algorithm.
    mutable unsigned long long int visitedNodesCount_ = 0;

    // The maximum depth that can be traversed in a Minimax tree.
    int explorationDepth_;

    Player player_;

    // Driver Function
    Move minimaxDriver(const OthelloBoard& board, Player player, int depth);
    // Worker Function
    float minimaxUsingAlphaBetaPruning(const OthelloBoard& board, Player player, unsigned int depth, float alpha, float beta, bool isMaximizingPlayerTurn);

    // Evaluation Function
    float evaluate(const OthelloBoard& board, Player player, std::vector<Move>& playerValidMoves, std::vector<Move>& opponentValidMoves) const;

    // Heuristic Functions
    inline float calculateCornerControl(const OthelloBoard& board, Player player) const;
    inline float calculatePotentialCorners(const OthelloBoard& board, Player player) const;
    inline float calculateStability(const OthelloBoard& board, Player player) const;
    inline float calculateMobility(const OthelloBoard& board, Player player, std::vector<Move>& playerValidMoves, std::vector<Move>& opponentValidMoves) const;
    inline float calculateCoinParity(const OthelloBoard& board, Player player) const;

    // This method examines all four axes 
    // around a disc to determine its stability.
    inline DiscStabilityState checkDiscStability(
        const OthelloBoard& board,
        int discsStabilityStateMap[BOARD_SIZE][BOARD_SIZE],
        int discStabilityIndex,
        Player player,
        int rowOfDisc,
        int colOfDisc
    ) const;

    // This auxiliary method examines one of the 
    // four axes of the disk in question at any given moment.
    inline DiscStabilityState checkAxisStability(
        const OthelloBoard& board,
        int discsStabilityStateMap[BOARD_SIZE][BOARD_SIZE],
        int discStabilityIndex,
        Player player,
        int rowOfDisc,
        int colOfDisc,
        std::pair<int, int> axisDirectionCoordinates
    ) const;

    // Heuristic's Weights
    float weightOfCornerControl_    = 3.0f;
    float weightOfPotentialCorners_ = 1.0f;
    float weightOfStability_        = 2.5f;
    float weightOfMobility_         = 2.0f;
    float weightOfCoinParity_       = 1.0f;
    
};

} // namespace othello