#include "Grid.h"
#include "Tetrimino.h"

constexpr const uint PLAYFIELD_START_COL = 4;
constexpr const uint PLAYFIELD_END_COL = 13;

sf::Color GRAY = Color(128, 128, 128);
sf::Color NAVY = Color(2, 7, 93);

char grid[NUM_CELLS_VERTICAL][NUM_CELLS_HORIZONTAL];

void Grid::initialize() {
    // From Tetris fandom wiki, the playfield is 10 blocks wide by 40 blocks tall
    // Tetriminos start falling from row 21 onwards
    // 
    // set the cells for the frame
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        grid[i][PLAYFIELD_START_COL - 1] = 1;
        grid[i][PLAYFIELD_END_COL + 1] = 1;
    }

    for (int j = PLAYFIELD_START_COL - 1; j <= PLAYFIELD_END_COL + 1; j++) {
        grid[NUM_CELLS_VERTICAL - 1][j] = 1;
    }

    // set the cells for inside the frame
    for (int i = 0; i < NUM_CELLS_VERTICAL - 1; i++) {
        for (int j = PLAYFIELD_START_COL; j <= PLAYFIELD_END_COL; j++) {
            grid[i][j] = 2;
        }
    }
}

void Grid::draw(RenderWindow& window) {
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        for (int j = 0; j < NUM_CELLS_HORIZONTAL; j++) {

            RectangleShape rect(Vector2f(CELL_SIZE, CELL_SIZE));
            rect.setPosition(Vector2f(j * CELL_SIZE, i * CELL_SIZE));

            sf::Color midnightBlue = Color(25, 25, 112);

            // color for outside the frame
            sf::Color c = Color::Black;

            switch (grid[i][j]) {
            case 1: {
                rect.setOutlineThickness(1);
                rect.setOutlineColor(midnightBlue);
                c = GRAY;
                break;
            }
            case 2: {
                rect.setOutlineThickness(1);
                rect.setOutlineColor(midnightBlue);
                c = NAVY;
                break;
            }
            case 3: {
                rect.setOutlineThickness(2);
                rect.setOutlineColor(midnightBlue);
                c = sf::Color::Magenta;
                break;
            }
            case 'I':
            case 'O':
            case 'T':
            case 'S':
            case 'Z':
            case 'J':
            case 'L': {
                rect.setOutlineThickness(1);
                rect.setOutlineColor(midnightBlue);
                c = Tetrimino::color_mp[grid[i][j]];
            }
            default: {
                break;
            }
            }
            rect.setFillColor(c);
            window.draw(rect);
        }
    }
}

// the default values for cells (inside the playfield) are 1 and 2, 3 is reserved 
inline bool isNonDefaultCell(char c) {
    return c > 3;
}

bool Grid::isCellOccuplied(int x, int y) {
    // this is the default y pos of a new shape
    if (y == -1) {
        return false;
    }

    // the floor is considered as "occupied"
    if (y == PLAYFIELD_END_ROW) {
        return true;
    }

    if (x == PLAYFIELD_START_COL-1 || x == PLAYFIELD_END_COL+1) {
        return true;
    }

    return isNonDefaultCell(grid[y][x]);
}

bool Grid::clearLine(int y) {
    bool should_clear = true;
    for (int j = PLAYFIELD_START_COL; j <= PLAYFIELD_END_COL && should_clear; j++) {
        if (!isNonDefaultCell(grid[y][j]))
            should_clear = false;
    }

    if (should_clear) {
        for (int j = PLAYFIELD_START_COL; j <= PLAYFIELD_END_COL; j++) {
            grid[y][j] = 3;
        }
        return true;
    }

    return false;
}

void Grid::dropBlocks() {
    // in each column, there are gaps which we must fill
    for (int j = PLAYFIELD_START_COL; j <= PLAYFIELD_END_COL; j++) {
        int k = NUM_CELLS_VERTICAL - 2;
        for (int i = NUM_CELLS_VERTICAL - 2; i >= 0; i--) {
            if (isNonDefaultCell(grid[i][j])) {
                grid[k--][j] = grid[i][j];
            }
        }

        for (; k >= 0; k--) {
            grid[k][j] = 2;
        }
    }

    // now we must recursively clear lines - note that we won't see an indicator (flash for this)
    bool cleared_any = false;
    for (int i = NUM_CELLS_VERTICAL - 2; i >= 0; i--) {
        cleared_any |= clearLine(i);
    }

    if (cleared_any) {
        dropBlocks();
    }
}
