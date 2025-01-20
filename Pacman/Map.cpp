#include "Map.h"

#define INDEX(row, col) (NUM_CELLS_HORIZONTAL * row + col)

char GRID[NUM_CELLS_VERTICAL][NUM_CELLS_HORIZONTAL] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
    { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0 },
    { 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
    { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0 },
    { 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

char DOT_STATUS[NUM_CELLS_VERTICAL][NUM_CELLS_HORIZONTAL] = { 0 };

std::unordered_set<int> Map::dotExcludeIndices = {
    INDEX(7, 9), INDEX(7, 11),
    INDEX(8, 0), INDEX(8, 1), INDEX(8,2), INDEX(8, 3), INDEX(8, 4), INDEX(8, 7), INDEX(8, 8), INDEX(8, 9), INDEX(8, 10), INDEX(8, 11), INDEX(8,12), INDEX(8, 13), INDEX(8, 17), INDEX(8, 18), INDEX(8, 19), INDEX(8, 20),
    INDEX(9, 7), INDEX(9, 10), INDEX(9, 13),
    INDEX(10, 6), INDEX(10, 7), INDEX(10, 9), INDEX(10, 10), INDEX(10, 11), INDEX(10, 13), INDEX(10, 14),
    INDEX(11, 6), INDEX(11, 7), INDEX(11, 9), INDEX(11, 10), INDEX(11, 11), INDEX(11, 13), INDEX(11, 14),
    INDEX(12, 0), INDEX(12, 1), INDEX(12, 2), INDEX(12, 3), INDEX(12, 7), INDEX(12, 13), INDEX(12, 17), INDEX(12, 18), INDEX(12, 19), INDEX(12, 20),
    INDEX(13, 0), INDEX(13, 1), INDEX(13, 2), INDEX(13, 3), INDEX(13, 7), INDEX(13, 8), INDEX(13, 9), INDEX(13, 10), INDEX(13, 11), INDEX(13, 12), INDEX(13, 13),   INDEX(13, 17), INDEX(13, 18), INDEX(13, 19), INDEX(13, 20),
    INDEX(14, 7), INDEX(14, 13),
    INDEX(17, 10)
};

Clock Map::c;

void Map::initialize() {
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        for (int j = 0; j < NUM_CELLS_HORIZONTAL; j++) {
            int idx = INDEX(i, j);
            if (GRID[i][j] == 1 && !dotExcludeIndices.count(idx)) {
                DOT_STATUS[i][j] = 1;
            }
        }
    }
    c.restart();
}

Time Map::getElapsedTime() {
    return c.getElapsedTime();
}


std::pair<float, float> Map::getCoordsForCells(int col, int row) {
    return std::make_pair(BOARD_OFFSET_LEFT + CELL_SIZE * col, BOARD_OFFSET_TOP + CELL_SIZE * row);
}

std::pair<float, float> Map::getCellsForCoords(int x, int y) {
    x -= BOARD_OFFSET_LEFT;
    y -= BOARD_OFFSET_TOP;

    return std::make_pair((float)x / CELL_SIZE, (float)y / CELL_SIZE);
}

std::pair<uint, uint> Map::getAdjacentTile(uint dir, uint col, uint row) {
    uint col2 = col, row2 = row;
    if (dir == 0) {
        col2 += 1;
    }
    else if (dir == 1) {
        row2 += 1;
    }
    else if (dir == 2) {
        col2 -= 1;
    }
    else if (dir == 3) {
        row2 -= 1;
    }

    return make_pair(col2, row2);
}

bool Map::isEmptyTile(uint dir, uint col, uint row) {
    auto [col2, row2] = Map::getAdjacentTile(dir, col, row);

    if (col2 < 0 || col2 >= NUM_CELLS_HORIZONTAL)
        return false;
    if (row2 < 0 || row2 >= NUM_CELLS_VERTICAL)
        return false;

    return GRID[row2][col2] == 1;
}

// given an entity's x and y coordinates, determine if we collide with the wall
// credit again to Servet Gulnaroglu on YT for this clever technique
bool Map::detectCollision(uint x, uint y) {
    auto [col, row] = Map::getCellsForCoords(x, y);

    if (GRID[(uint)std::floor(row)][(uint)std::floor(col)] == 0)
        return true;


    if (GRID[(uint)std::floor(row)][(uint)std::floor(col + 0.999)] == 0) {
        return true;
    }

    if (GRID[(uint)std::floor(row + 0.999)][(uint)std::floor(col)] == 0) {
        return true;
    }

    if (GRID[(uint)std::floor(row + 0.999)][(uint)std::floor(col + 0.999)] == 0) {
        return true;
    }

    return false;
}


void Map::drawWalls(RenderWindow& window) {
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        for (int j = 0; j < NUM_CELLS_HORIZONTAL; j++) {
            if (GRID[i][j] == 0) {
                RectangleShape r(Vector2f(CELL_SIZE, CELL_SIZE));
                r.setFillColor(Color::Blue);

                auto [x, y] = getCoordsForCells(j, i);
                r.setPosition(x, y);

                window.draw(r);
            }
        }
    }
}

void Map::drawInnerWalls(RenderWindow& window) {
    // credit to Servet Gulnaroglu on YT for this clever technique
    // this essentially creates a "space" inside the walls which results in a nice wall boundary

    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        for (int j = 0; j < NUM_CELLS_HORIZONTAL; j++) {

            if (j > 0 && GRID[i][j - 1] == 0) {
                RectangleShape r(Vector2f(WALL_INNER_WIDTH + WALL_OFFSET, WALL_INNER_WIDTH));
                r.setFillColor(Color::Black);

                auto [x, y] = getCoordsForCells(j, i);
                r.setPosition(Vector2f(x, WALL_OFFSET + y));
                window.draw(r);
            }

            if (j < NUM_CELLS_HORIZONTAL - 1 && GRID[i][j + 1] == 0) {
                RectangleShape r(Vector2f(WALL_INNER_WIDTH + WALL_OFFSET, WALL_INNER_WIDTH));
                r.setFillColor(Color::Black);

                auto [x, y] = getCoordsForCells(j, i);
                r.setPosition(Vector2f(WALL_OFFSET + x, WALL_OFFSET + y));
                window.draw(r);
            }

            if (i > 0 && GRID[i - 1][j] == 0) {
                RectangleShape r(Vector2f(WALL_INNER_WIDTH, WALL_INNER_WIDTH + WALL_OFFSET));
                r.setFillColor(Color::Black);

                auto [x, y] = getCoordsForCells(j, i);
                r.setPosition(Vector2f(WALL_OFFSET + x, y));
                window.draw(r);
            }

            if (i < NUM_CELLS_VERTICAL - 1 && GRID[i + 1][j] == 0) {
                RectangleShape r(Vector2f(WALL_INNER_WIDTH, WALL_INNER_WIDTH + WALL_OFFSET));
                r.setFillColor(Color::Black);

                auto [x, y] = getCoordsForCells(j, i);
                r.setPosition(Vector2f(WALL_OFFSET + x, WALL_OFFSET + y));
                window.draw(r);
            }
        }
    }
}

void Map::drawDots(sf::RenderWindow& window) {
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        for (int j = 0; j < NUM_CELLS_HORIZONTAL; j++) {
            if (DOT_STATUS[i][j] == 1) {
                CircleShape c(2.0f);
                c.setFillColor(Color::Yellow);
                auto [x, y] = getCoordsForCells(j, i);

                c.setPosition(Vector2f(x + CELL_SIZE / 2, y + CELL_SIZE / 2));
                window.draw(c);
            }
        }
    }
}

void Map::reset() {
    c.restart();
}
