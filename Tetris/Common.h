#pragma once

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

typedef unsigned int uint;
typedef vector<int> VI;

constexpr const uint BOARD_WIDTH = 720;
constexpr const uint BOARD_HEIGHT = 960;

constexpr const uint CELL_SIZE = 40;
constexpr const uint NUM_CELLS_HORIZONTAL = BOARD_WIDTH / CELL_SIZE;
constexpr const uint NUM_CELLS_VERTICAL = BOARD_HEIGHT / CELL_SIZE;

constexpr const uint PLAYFIELD_END_ROW = NUM_CELLS_VERTICAL - 1;
