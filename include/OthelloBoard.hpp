#pragma once

#include <array>
#include <cstdint>

namespace othello {

// Board size for a standard Othello game (8x8)
static constexpr std::uint8_t BOARD_SIZE = 8;

// Define a type alias for the game board. Board type used by the whole project.
// Using a fixed-size array makes the board simple, predictable, and fast.
using Board = std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE>;

}
