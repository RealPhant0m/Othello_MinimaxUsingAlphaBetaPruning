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
                                (int _dr, int _dc, int directionIndex) {
                            
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

inline OthelloAI::DiscStabilityState OthelloAI::checkDiscStability(
    const OthelloBoard& board,
    int discsStabilityStateMap[BOARD_SIZE][BOARD_SIZE],
    int discStateIndex,
    Player player,
    int rowOfDisc,
    int colOfDisc) const 
{
    DiscStabilityState discStatusAlongAxis;
    for (int i = 0; i < 4; i++) {
        discStatusAlongAxis = checkAxisStability(board, discsStabilityStateMap, discStateIndex, player, rowOfDisc, colOfDisc, directions[i]);
        if (discStatusAlongAxis == Unstable)
            return Unstable;
        if (discStatusAlongAxis == SemiStable)
            return SemiStable;
    }
    return Stable;
}

inline OthelloAI::DiscStabilityState OthelloAI::checkAxisStability(
    const OthelloBoard& board,
    int discsStabilityStateMap[BOARD_SIZE][BOARD_SIZE],
    int discStateIndex,
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
        return Stable;
    else if (discsStabilityStateMap[forwardRow][forwardCol] == discStateIndex || discsStabilityStateMap[backwardRow][backwardCol] == discStateIndex)
        return Stable;

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
            return Unstable;
    }

    arrayIndex = playerIndex;
    while (axisLine[arrayIndex] == 'P')
        arrayIndex++;
    if (axisLine[arrayIndex] == 'O') {
        arrayIndex = playerIndex - 1;
        while (axisLine[arrayIndex] == 'P')
            arrayIndex--;
        if (axisLine[arrayIndex] == 'E')
            return Unstable;
    }

    bool didTheEmptyCellChange = false;
    while (true) {
        arrayIndex = 1;
        if (axisLine[playerIndex] != 'P')
            return SemiStable;
        didTheEmptyCellChange = false;
        while (axisLine[arrayIndex] != 'X' && axisLine[arrayIndex] != 'E')
            arrayIndex++;

        // When there is no empty cell on the 
        // desired axis and the entire axis is 
        // occupied by discs, then the desired 
        // disc cannot be flipped on that axis.
        if (axisLine[arrayIndex] == 'X')
            return Stable;
        
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
            return Stable;
    }
    return Unstable;
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
