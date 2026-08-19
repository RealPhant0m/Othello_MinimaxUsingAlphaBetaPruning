#include "OthelloAI.hpp"

#include <algorithm>

namespace othello {

OthelloAI::OthelloAI(Player player, unsigned int explorationDepth = 6) : player_(player) {
    setExplorationDepth(explorationDepth);
}

void OthelloAI::setExplorationDepth(int depth = 6) {
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
                                (int _dr, int _dc, int directionIndex) 
    {
        if (boardFormat[_dr][_dc] == OthelloBoard::toChar(OthelloBoard::getOpponent(player))) {
            while (OthelloBoard::isInside(_dr, _dc) && boardFormat[_dr][_dc] == OthelloBoard::toChar(OthelloBoard::getOpponent(player))) {
                _dr += directions[directionIndex].first;
                _dc += directions[directionIndex].second;
            }
            if (OthelloBoard::isInside(_dr, _dc) && boardFormat[_dr][_dc] == OthelloBoard::toChar(player))
                playerPotentialCornersCount++;
        } else if (boardFormat[_dr][_dc] == OthelloBoard::toChar(player)) {
            while (OthelloBoard::isInside(_dr, _dc) && boardFormat[_dr][_dc] == OthelloBoard::toChar(player)) {
                _dr += directions[directionIndex].first;
                _dc += directions[directionIndex].second;
            }
            if (OthelloBoard::isInside(_dr, _dc) && boardFormat[_dr][_dc] == OthelloBoard::toChar(OthelloBoard::getOpponent(player)))
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

inline float OthelloAI::calculateStability(const OthelloBoard& board, Player player) const {
    int playerStableDiscsCount = 0;
    int opponentStableDiscsCount = 0;

    const auto& boardFormat = board.board();
    char _player = OthelloBoard::toChar(player);
    char _opponent = OthelloBoard::toChar(OthelloBoard::getOpponent(player));

    int discsStabilityStateMap[BOARD_SIZE][BOARD_SIZE] = 
        {{-2,-2,-2,-2,-2,-2,-2,-2}, // -2: Cell with unknown status
         {-2,-2,-2,-2,-2,-2,-2,-2}, // -4: This Cell Contains Opponent's Unstable Disc
         {-2,-2,-2,-2,-2,-2,-2,-2}, // -3: This Cell Contains Opponent's SemiStable Disc
         {-2,-2,-2,-2,-2,-2,-2,-2}, // -1: This Cell Contains Opponent's Stable Disc
         {-2,-2,-2,-2,-2,-2,-2,-2}, //  1: This Cell Contains Player's Stable Disc 
         {-2,-2,-2,-2,-2,-2,-2,-2}, //  3: This Cell Contains Player's SemiStable Disc
         {-2,-2,-2,-2,-2,-2,-2,-2}, //  4: This Cell Contains Player's Unstable Disc
         {-2,-2,-2,-2,-2,-2,-2,-2}};

    auto runDiscChecker = [this, &discsStabilityStateMap, &boardFormat, &board] 
        (Player player, int rowOfDisc, int colOfDisc, int stableIndex, int semistableIndex, int unstableIndex)
    {
        DiscStabilityState discStatus;
        discStatus = checkDiscStability(board, discsStabilityStateMap, stableIndex, player, rowOfDisc, colOfDisc);
        if (discStatus == OthelloAI::Stable)
            discsStabilityStateMap[rowOfDisc][colOfDisc] = stableIndex;
        else if (discStatus == OthelloAI::SemiStable)
            discsStabilityStateMap[rowOfDisc][colOfDisc] = semistableIndex;
        else if (discStatus == OthelloAI::Unstable)
            discsStabilityStateMap[rowOfDisc][colOfDisc] = unstableIndex;
    };

    for (int col = 0; col < BOARD_SIZE; col++) {
        //Check the top and Bottom Edge
        if (boardFormat[0][col] == _player)
            runDiscChecker(player, 0, col, 1, 3, 4);
        else if (boardFormat[0][col] == _opponent)
            runDiscChecker(OthelloBoard::getOpponent(player), 0, col, -1, -3, -4);
        if (boardFormat[7][col] == _player) 
            runDiscChecker(player, 7, col, 1, 3, 4);
        else if (boardFormat[7][col] == _opponent)
            runDiscChecker(OthelloBoard::getOpponent(player), 7, col, -1, -3, -4);
    }
    for (int col = BOARD_SIZE - 1; col >= 0; col--) {
        //Check the top and Bottom Edges in reverse order
        if (boardFormat[0][col] == _player)
            runDiscChecker(player, 0, col, 1, 3, 4);
        else if (boardFormat[0][col] == _opponent)
            runDiscChecker(OthelloBoard::getOpponent(player), 0, col, -1, -3, -4);
        if (boardFormat[7][col] == _player) 
            runDiscChecker(player, 7, col, 1, 3, 4);
        else if (boardFormat[7][col] == _opponent)
            runDiscChecker(OthelloBoard::getOpponent(player), 7, col, -1, -3, -4);
    }

    for (int row = 0; row < BOARD_SIZE; row++) {
        //Check the Left and Right Edges
        if (boardFormat[row][0] == _player)
            runDiscChecker(player, row, 0, 1, 3, 4);
        else if (boardFormat[row][0] == _opponent)
            runDiscChecker(OthelloBoard::getOpponent(player), row, 0, -1, -3, -4);
        if (boardFormat[row][7] == _player)
            runDiscChecker(player, row, 7, 1, 3, 4);
        else if (boardFormat[row][7] == _opponent)
            runDiscChecker(OthelloBoard::getOpponent(player), row, 7, -1, -3, -4);
    }
    for (int row = BOARD_SIZE - 1; row >= 0; row--) {
        //Check the Left and Right Edges in reverse order
        if (boardFormat[row][0] == _player)
            runDiscChecker(player, row, 0, 1, 3, 4);
        else if (boardFormat[row][0] == _opponent)
            runDiscChecker(OthelloBoard::getOpponent(player), row, 0, -1, -3, -4);
        if (boardFormat[row][7] == _player)
            runDiscChecker(player, row, 7, 1, 3, 4);
        else if (boardFormat[row][7] == _opponent)
            runDiscChecker(OthelloBoard::getOpponent(player), row, 7, -1, -3, -4);
    }

    for (int row = 1; row < BOARD_SIZE - 1; row++) {
        for (int col = 1; col < BOARD_SIZE - 1; col++) {
            if (boardFormat[row][col] == _player)
                runDiscChecker(player, row, col, 1, 3, 4);
            else if (boardFormat[row][col] == _opponent)
                runDiscChecker(OthelloBoard::getOpponent(player), row, col, -1, -3, -4);
        }
    }

    for (const auto& row : discsStabilityStateMap) {
        for (int cell : row) {
            if (cell == 1)
                playerStableDiscsCount++;
            else if (cell == -1)
                opponentStableDiscsCount++;
        }
    }

    if (playerStableDiscsCount + opponentStableDiscsCount != 0)
        return ((static_cast<float>(playerStableDiscsCount - opponentStableDiscsCount) / static_cast<float>(playerStableDiscsCount + opponentStableDiscsCount)) * 100);
    return 0;
}

inline OthelloAI::DiscStabilityState OthelloAI::checkDiscStability(
    const OthelloBoard& board,
    int discsStabilityStateMap[BOARD_SIZE][BOARD_SIZE],
    int discStabilityIndex,
    Player player,
    int rowOfDisc,
    int colOfDisc) const 
{
    DiscStabilityState discStatusAlongAxis;
    for (int i = 0; i < 4; i++) {
        discStatusAlongAxis = checkAxisStability(board, discsStabilityStateMap, discStabilityIndex, player, rowOfDisc, colOfDisc, directions[i]);
        if (discStatusAlongAxis == OthelloAI::Unstable)
            return OthelloAI::Unstable;
        if (discStatusAlongAxis == OthelloAI::SemiStable)
            return OthelloAI::SemiStable;
    }
    return OthelloAI::Stable;
}

inline OthelloAI::DiscStabilityState OthelloAI::checkAxisStability(
    const OthelloBoard& board,
    int discsStabilityStateMap[BOARD_SIZE][BOARD_SIZE],
    int discStabilityIndex,
    Player player,
    int rowOfDisc,
    int colOfDisc,
    std::pair<int, int> axisDirectionCoordinates) const 
{   
    int dr = axisDirectionCoordinates.first;
    int dc = axisDirectionCoordinates.second;

    int forwardRow  = rowOfDisc + dr;
    int forwardCol  = colOfDisc + dc;
    int backwardRow = rowOfDisc - dr;
    int backwardCol = colOfDisc - dc;

    // When a disc is placed on the edge of the board, then at least 3 of its 4 axes 
    // have one of their directions towards the outside of the board, which means that 
    // along any of these axes, the opponent cannot flip the disc in question. 
    // Therefore, the disc in question is stable only on that axis.
    if (!OthelloBoard::isInside(forwardRow, forwardCol) || !OthelloBoard::isInside(backwardRow, backwardCol))
        return OthelloAI::Stable;
    else if (discsStabilityStateMap[forwardRow][forwardCol] == discStabilityIndex || discsStabilityStateMap[backwardRow][backwardCol] == discStabilityIndex)
        return OthelloAI::Stable;

    const auto& boardFormat = board.board();
    char _player = OthelloBoard::toChar(player);
    char _opponent = OthelloBoard::toChar(OthelloBoard::getOpponent(player));
    char _empty = OthelloBoard::toChar(Player::Empty);

    // This array represents status of each cell of the board along the axis. 
    // U : Unknown status symbol.
    // X : Out of bounds symbol. this means the coordinate is outside of the board.
    // P : Own disc(player's disc) with unknown status.
    // O : Opponent's disc with unknown status.
    // E : Empty cell symbol.
    char axisLine[10] = {'U', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'U', 'U'};

    int r = rowOfDisc;
    int c = colOfDisc;
    int arrayIndex = 0;

    // Moving backward along the axis until reaching a point outside the board.
    while (OthelloBoard::isInside(r, c)) {
        r -= dr;
        c -= dc;
    }
    axisLine[arrayIndex] = 'X';
    r += dr;
    c += dc;
    int playerIndex; // Indicates the position of the player's disc in the axisLine array.

    while (OthelloBoard::isInside(r, c)) {
        if (boardFormat[r][c] == _player)
            axisLine[++arrayIndex] = 'P';
        else if (boardFormat[r][c] == _opponent)
            axisLine[++arrayIndex] = 'O';
        else if (boardFormat[r][c] == _empty)
            axisLine[++arrayIndex] = 'E';

        if (r == rowOfDisc && c == colOfDisc)
            playerIndex = arrayIndex;
        
        r += dr;
        c += dc;
    }

    // When we exit the while loop above, the values ​​of r and c are values ​
    // ​that are outside the dimensions of the board. So we have reached 
    // the end of that axis and we need to place a value in axisLine 
    // that represents the outside of the board.
    axisLine[++arrayIndex] = 'X';

    arrayIndex = playerIndex;
    while (axisLine[arrayIndex] == 'P')
        arrayIndex--;
    if (axisLine[arrayIndex] == 'O') {
        arrayIndex = playerIndex + 1;
        while (axisLine[arrayIndex] == 'P')
            arrayIndex++;
        if (axisLine[arrayIndex] == 'E')
            return OthelloAI::Unstable;
    }

    arrayIndex = playerIndex;
    while (axisLine[arrayIndex] == 'P')
        arrayIndex++;
    if (axisLine[arrayIndex] == 'O') {
        arrayIndex = playerIndex - 1;
        while (axisLine[arrayIndex] == 'P')
            arrayIndex--;
        if (axisLine[arrayIndex] == 'E')
            return OthelloAI::Unstable;
    }

    bool didTheEmptyCellChange = false;
    while (true) {
        arrayIndex = 1;
        if (axisLine[playerIndex] != 'P')
            return OthelloAI::SemiStable;
        didTheEmptyCellChange = false;
        while (axisLine[arrayIndex] != 'X' && axisLine[arrayIndex] != 'E')
            arrayIndex++;

        // When there is no empty cell on the 
        // desired axis and the entire axis is 
        // occupied by discs, then the desired 
        // disc cannot be flipped on that axis.
        if (axisLine[arrayIndex] == 'X')
            return OthelloAI::Stable;
        
        if (axisLine[arrayIndex] == 'E') {
            char disc;
            int counter = arrayIndex + 1;
            if (axisLine[counter] != 'X' && axisLine[counter] != 'E') {
                disc = axisLine[counter];
                while (axisLine[counter] == disc)
                    counter++;
                if (axisLine[counter] == ((disc == 'P')?'O':'P')) {
                    counter--;
                    while (axisLine[counter] == disc) {
                        axisLine[counter] = (disc == 'P')?'O':'P';
                        counter--;
                    }
                    axisLine[arrayIndex] = (disc == 'P')?'O':'P';
                    didTheEmptyCellChange = true;
                }
            }
            counter = arrayIndex;
            if (axisLine[arrayIndex] != 'E') {
                disc = axisLine[arrayIndex];
                counter--;
                if (axisLine[counter] != 'X' && axisLine[counter] != 'E' && axisLine[counter] == ((disc == 'P')?'O':'P')) {
                    while (axisLine[counter] == ((disc == 'P')?'O':'P'))
                        counter--;
                    if (axisLine[counter] == disc) {
                        counter++;
                        while (axisLine[counter] == ((disc == 'P')?'O':'P')) {
                            axisLine[counter] = disc;
                            counter++;
                        }
                    }
                }
            } else if (axisLine[arrayIndex] == 'E') {
                counter--;
                if (axisLine[counter] != 'X' && axisLine[counter] != 'E') {
                    disc = axisLine[counter];
                    while (axisLine[counter] == disc)
                        counter--;
                    if (axisLine[counter] == ((disc == 'P')?'O':'P')) {
                        counter++;
                        while (axisLine[counter] == disc) {
                            axisLine[counter] = (disc == 'P')?'O':'P';
                            counter++;
                        }
                        axisLine[arrayIndex] = (disc == 'P')?'O':'P';
                        didTheEmptyCellChange = true;
                    }
                }
            }
        }
        if (axisLine[playerIndex] == 'P' && !didTheEmptyCellChange)
            return OthelloAI::Stable;
    }
    return OthelloAI::Unstable;
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

Move OthelloAI::chooseMove(const OthelloBoard& board) {
    visitedNodesCount_ = 0; // Reset the visited nodes count before starting the search.

    Move bestMove = minimaxDriver(board, player_, explorationDepth_);

    return bestMove;
}

Move OthelloAI::minimaxDriver(const OthelloBoard& board, Player player, int depth) {
    visitedNodesCount_++;
    Move bestMove;
    bestMove.row = -1;
    bestMove.col = -1;
    float bestMoveScore = NEGATIVE_INFINITY;
    auto playerValidMoves = board.getValidMoves(player);
    float alpha = NEGATIVE_INFINITY;
    float beta = POSITIVE_INFINITY;

    for (const auto& move : playerValidMoves) {
        const OthelloBoard nextBoard = OthelloBoard(board.applyMove(move, player));
        float moveScore = minimaxUsingAlphaBetaPruning(nextBoard, player, depth - 1, alpha, beta, false);

        if (moveScore > bestMoveScore) {
            bestMoveScore = moveScore;
            bestMove = move;
        }

        alpha = std::max(alpha, bestMoveScore);

        // Prune the remaining branches when beta <= alpha
        if (beta <= alpha)
            break;
    }
    return bestMove;
}

float OthelloAI::minimaxUsingAlphaBetaPruning(
    const OthelloBoard& board, 
    Player player, 
    unsigned int depth, 
    float alpha, 
    float beta, 
    bool isMaximizingPlayerTurn) 
{
    visitedNodesCount_++;
    
    std::vector<Move> playerValidMoves = board.getValidMoves(player);
    std::vector<Move> opponentValidMoves = board.getValidMoves(OthelloBoard::getOpponent(player));

    if (depth == 0 && board.isGameOver(playerValidMoves, opponentValidMoves))
        //when Leaf node reached and game is over, returns the evaluation of the board.
        return evaluate(board, player, playerValidMoves, opponentValidMoves);
    else if (board.isGameOver(playerValidMoves, opponentValidMoves))
        return ((weightOfCoinParity_       +
                 weightOfCornerControl_    +
                 weightOfMobility_         +
                 weightOfPotentialCorners_ +
                 weightOfStability_) * calculateCoinParity(board, player));
    
    if (playerValidMoves.empty() && isMaximizingPlayerTurn)
        isMaximizingPlayerTurn = false;
    if (opponentValidMoves.empty() && !isMaximizingPlayerTurn)
        isMaximizingPlayerTurn = true;

    float bestMoveScore; 
    bestMoveScore = (isMaximizingPlayerTurn)?NEGATIVE_INFINITY:POSITIVE_INFINITY;

    if (isMaximizingPlayerTurn) {
        for (const auto& move : playerValidMoves) {
            const OthelloBoard nextBoard = OthelloBoard(board.applyMove(move, player));
            float moveScore = minimaxUsingAlphaBetaPruning(nextBoard, player, depth - 1, alpha, beta, false);

            if (moveScore > bestMoveScore)
                bestMoveScore = moveScore;
            
            alpha = std::max(alpha, bestMoveScore);

            // Prune the remaining branches when beta <= alpha
            if (beta <= alpha)
                break;
        }
        return bestMoveScore;
    } 
    else {
        for (const auto& move : opponentValidMoves) {
            const OthelloBoard nextBoard = OthelloBoard(board.applyMove(move, player));
            float moveScore = minimaxUsingAlphaBetaPruning(nextBoard, player, depth - 1, alpha, beta, true);

            if (moveScore < bestMoveScore)
                bestMoveScore = moveScore;
            
            beta = std::min(beta, bestMoveScore);

            // Prune the remaining branches when beta <= alpha
            if (beta <= alpha)
                break;
        }
        return bestMoveScore;
    }
}

} // namespace othello
