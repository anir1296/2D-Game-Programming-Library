#pragma once

#include "Common.h"
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <unordered_set>

typedef unsigned int uint;

constexpr const uint NUM_CELLS_HORIZONTAL = 21;
constexpr const uint NUM_CELLS_VERTICAL = 23;

constexpr const uint CELL_SIZE = 24;

constexpr const uint BOARD_OFFSET_TOP = 80;
constexpr const uint BOARD_OFFSET_LEFT = 48;

constexpr const float WALL_INNER_WIDTH = CELL_SIZE / 1.6;
constexpr const float WALL_OFFSET = (CELL_SIZE - WALL_INNER_WIDTH) / 2;

extern char GRID[NUM_CELLS_VERTICAL][NUM_CELLS_HORIZONTAL];
extern char DOT_STATUS[NUM_CELLS_VERTICAL][NUM_CELLS_HORIZONTAL];

class Map {
public:
    static void initialize();
    static void drawWalls(sf::RenderWindow &window);
    static void drawInnerWalls(sf::RenderWindow &window);
    static void drawDots(sf::RenderWindow& window);
    static std::pair<float, float> getCoordsForCells(int col, int row);
    static std::pair<float, float> getCellsForCoords(int x, int y);
    static bool detectCollision(uint x, uint y);
    static std::pair<uint, uint> getAdjacentTile(uint dir, uint col, uint row);
    static bool isEmptyTile(uint dir, uint col, uint row);
    static sf::Time getElapsedTime();
    static void reset();
private:
    static std::unordered_set<int> dotExcludeIndices;
    static sf::Clock c;
};