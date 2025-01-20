#include "Ghost.h"
#include "Map.h"
#include "Pacman.h"

uint startingCells[4][2] = {
	{8, 10},
	{10, 9},
	{11, 10},
	{10, 11}
};

uint exit_cell_x = 10, exit_cell_y = 8;
uint jail_cell_start_x = 9, jail_cell_end_x = 11;
uint jail_cell_start_y = 9, jail_cell_end_y = 10;

uint dead_end_cell_1_x = 0, dead_end_cell_1_y = 10;
uint dead_end_cell_2_x = 20, dead_end_cell_2_y = 10;

Texture* Ghost::ghost_textures;
Sprite Ghost::ghost_sprites[4];

float Ghost::speed[4];
uint Ghost::dirs[4];
float Ghost::pos[4][2];
bool Ghost::isActive[4];

Ghost::Ghost() {
	ghost_textures = new Texture[4]();

	for (int i = 0; i < 4; i++) {
		string texture_filename = "sprites/ghost_" + std::to_string(i) + ".png";
		ghost_textures[i].loadFromFile(texture_filename);
		ghost_sprites[i].setTexture(ghost_textures[i]);
	}
	reset();
}

Ghost::~Ghost() {
}

std::pair<int, int> Ghost::getNextCoordinates(uint dir, uint i) {
	uint x = pos[i][0], y = pos[i][1];
	uint newX = pos[i][0], newY = pos[i][1];

	if (dir == 0) {
		newX = x + speed[i];
	}
	else if (dir == 1) {
		newY = y + speed[i];
	}
	else if (dir == 2) {
		newX = x - speed[i];
	}
	else if (dir == 3) {
		newY = y - speed[i];
	}

	return std::make_pair(newX, newY);
}

void Ghost::draw(RenderWindow& window) {
	for (int i = 0; i < 4; i++) {
		window.draw(ghost_sprites[i]);
	}
}

std::pair<uint, uint> getAdjustedCell(uint dir, uint x, uint y) {
	auto [col, row] = Map::getCellsForCoords(x, y);
	if (dir == 0) {
		col = std::ceil(col);
	}
	else if (dir == 1) {
		row = std::ceil(row);
	}
	else if (dir == 2) {
		col = std::floor(col);
	}
	else if (dir == 3) {
		row = std::floor(row);
	}

	return std::make_pair(col, row);
}

sf::FloatRect Ghost::getGhostBoundingRect(uint i) {
	return ghost_sprites[i].getGlobalBounds();
}

bool Ghost::isGhostInPrison(uint i) {
	auto [col, row] = Map::getCellsForCoords(pos[i][0], pos[i][1]);
	col = std::floor(col), row = std::floor(row);

	return col >= jail_cell_start_x && col <= jail_cell_end_x && row >= jail_cell_start_y && row <= jail_cell_end_y;
}

// the actual semantics of the ghosts' movements are fairly complicated and each individual
// ghost has unique mechanics - as explained in this excellent article:	https://gameinternals.com/understanding-pac-man-ghost-behavior
// this is a much simpler implementation than the algorithm in the original game

void Ghost::move() {
	auto [pac_col, pac_row] = Pacman::getPacmanCell();
	pac_col = std::floor(pac_col), pac_row = std::floor(pac_row);

	for (int i = 0; i < 4; i++) {
		if (isActive[i]) {

			uint dir = dirs[i];
			uint target_col = pac_col, target_row = pac_row;

			if (isGhostInPrison(i)) {
				// move the ghost to the prison exit
				target_col = exit_cell_x, target_row = exit_cell_y;
			}

			auto [col, row] = getAdjustedCell(dir, pos[i][0], pos[i][1]);
			auto [newX, newY] = getNextCoordinates(dirs[i], i);
			auto [new_col, new_row] = getAdjustedCell(dir, newX, newY);

			bool leaving_tile = col != new_col || row != new_row;

			uint numTurns = 0;
			vector<int> possible_turns;
			for (int j = 0; j < 4; j++) {
				// if we are at the jail entrance, we don't want to consider the down direction
				if (j == 1 && col == exit_cell_x && row == exit_cell_y)
					continue;

				if (Map::isEmptyTile(j, col, row)) {
					numTurns++;
					// we never consider the opposite direction of the ghost to be a viable direction to switch
					if ((dirs[i] + 2) % 4 == j)
						continue;
					possible_turns.push_back(j);
				}
			}

			bool juncture_tile = numTurns > 2;

			if (leaving_tile && juncture_tile) {
				// we are leaving a juncture tile and we must make a decision on where to turn
				// calculate the straight line distance from the candidate tile to the target tile - the tile w/ the shortest distance wins
				// however there is a 10% chance we choose a non-optimal tile - this is a randomization which makes the ghosts more unpredictable

				uint target_dir = dirs[i];
				double target_dist = INT_MAX;
				for (int k = 0; k < possible_turns.size(); k++) {

					uint temp_dir = possible_turns[k];

					auto [col2, row2] = Map::getAdjacentTile(temp_dir, col, row);

					int col_diff = col2 - target_col;
					int row_diff = row2 - target_row;

					double dist = std::sqrt(col_diff * col_diff + row_diff * row_diff);
					if (dist < target_dist) {
						target_dist = dist;
						target_dir = temp_dir;
					}
				}

				int rand = std::rand() % 10;
				uint chosen_dir = target_dir;
				if (rand == 0) {
					for (int k = 0; k < possible_turns.size(); k++) {
						if (possible_turns[k] != target_dir) {
							chosen_dir = possible_turns[k];
						}
					}
				}

				dirs[i] = chosen_dir;
				auto [newX, newY] = getNextCoordinates(dirs[i], i);
				pos[i][0] = newX; pos[i][1] = newY;
				ghost_sprites[i].setPosition(newX, newY);
			}
			else {
				// we are not at a juncture tile
				// just continue in current direction if we can, otherwise make the default turn at a wall

				auto [newX, newY] = getNextCoordinates(dirs[i], i);
				if (!Map::detectCollision(newX, newY)) {
					pos[i][0] = newX; pos[i][1] = newY;
					ghost_sprites[i].setPosition(newX, newY);
				}
				else {
					if (col == exit_cell_x && row == exit_cell_y) {
						// ghosts will either go left or right once they exit the jail
						dirs[i] = (std::rand() % 2) ? 2 : 0;
					}
					else {
						// if we arrive here we should only have 1 possible direction
						// the only exception to this are the dead end cells 
						if (new_col == dead_end_cell_1_x && new_row == dead_end_cell_1_y) {
							dirs[i] = 0;
						} 
						else if (new_col == dead_end_cell_2_x && new_row == dead_end_cell_2_y) {
							dirs[i] = 2;
						}
						else {
							_ASSERT(possible_turns.size() == 1);
							dirs[i] = possible_turns[0];

						}
					}
				}
			}
		}
		else {
			auto t = Map::getElapsedTime();
			if (t.asSeconds() > (5 * i)) {
				isActive[i] = true;
			}
		}
	}
}

void Ghost::reset() {
	for (int i = 0; i < 4; i++) {
		int row = startingCells[i][0], col = startingCells[i][1];
		auto [x, y] = Map::getCoordsForCells(col, row);
		pos[i][0] = x, pos[i][1] = y;
		ghost_sprites[i].setPosition(x, y);
		
		isActive[i] = (i == 0);

		if (i == 0) {
			dirs[i] = (std::rand() % 2) ? 2 : 0;
			speed[i] = 2;
		}
		else if (i == 1) {
			dirs[i] = 0;
			speed[i] = 2;
		}
		else if (i == 2) {
			dirs[i] = 3;
			speed[i] = 2;
		}
		else if (i == 3) {
			dirs[i] = 2;
			speed[i] = 2;
		}
	}
}