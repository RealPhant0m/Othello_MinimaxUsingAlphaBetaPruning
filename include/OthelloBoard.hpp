#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <utility>
#include <string>

namespace othello {

// Board size for a standard Othello game (8x8)
static constexpr int BOARD_SIZE = 8;

// Define a type alias for the game board. Board type used by the whole project.
// Using a fixed-size array makes the board simple, predictable, and fast.
using Board = std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>;

// One legal move in the game.
// row/col = destination position
// flips = directions that will be flipped if this move is played
struct Move {
    short int row;
    short int col;
    std::vector<int> flippableDirections;
};

enum class Player : char {
    Empty = '0',
    White = '1',
    Black = '2'
};

// Representation of delta row and delta column,
// whose values ​​vary from -1 to 1, as an array of pairs called "directions"
constexpr std::pair<int, int> directions[8] = {  //  (-1,-1)  (-1,0)   (-1,1)
    std::make_pair(-1,0),                        //        *     *     *
    std::make_pair(-1,1),                        //          *   *   *
    std::make_pair(0,1),                         //            * * *
    std::make_pair(1,1),                         // (0,-1) * * * D * * * (0,1)    <=== 8 Directions for a Disc
    std::make_pair(1,0),                         //            * * *
    std::make_pair(1,-1),                        //          *   *   *
    std::make_pair(0,-1),                        //        *     *     *
    std::make_pair(-1,-1)                        //   (1,-1)   (1,0)   (1,1)
};

// Core game engine.
// This class owns the board state and the rules of Othello.
// It does not make decisions; it only knows how to play the game.
class OthelloBoard {
public:

    OthelloBoard();
    explicit OthelloBoard(const std::string& boardString);
    explicit OthelloBoard(const Board& board);

    // Board state helpers.
    void convertBoardFormat(const std::string& boardString);
    std::string convertBoardFormat() const;
    const Board& board() const;

    // Core game rules.
    std::vector<Move> getValidMoves(Player player) const;

    // Simple utility functions.
    static char toChar(Player player);
    bool isOpponent(Player player, int row, int col) const;
    static bool isInside(int row, int col);

private:
    Board board_{};

    std::vector<int> getFlippableDirections(Player player, int row, int col) const;

};

}
