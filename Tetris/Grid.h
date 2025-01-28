#pragma once
#include "Common.h"

extern char grid[NUM_CELLS_VERTICAL][NUM_CELLS_HORIZONTAL];

enum CELL_TYPE {
    OUTSIDE = 1, // corresponds to outside the playing field
    FRAME = 2, // corresponds to the border
    EMPTY = 3 // corresponds to an empty cell inside the frame
};

class Grid {
public:
	static void initialize();
	static void draw(RenderWindow& window);
    static bool isCellOccuplied(int x, int y);
    static bool clearLine(int y);
    static void dropBlocks();
};
