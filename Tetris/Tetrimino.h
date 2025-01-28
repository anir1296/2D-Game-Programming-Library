#pragma once
#include <string>
#include <iostream>
#include <unordered_map>
#include "Common.h"

class Tetrimino {
public:
	Tetrimino(char type);
	static void initialize();
	void draw(RenderWindow& window);
	bool move(uint dir);
	bool drop(bool fast);
	void rotate(bool right);

	static unordered_map<char, sf::Color> color_mp;
	bool isAlive = true;

private:

	char type = 'I';

	// 3D array for rotations - # of rotations varies by shape
	vector<vector<VI>> rotations;

	// N represents the bounding box size
	// shapes I and O have size 4 so we have a 4x4 grid
	// other shapes have size 3
	uint N = 0;

	uint NUM_ROTATIONS = 0;
	uint rot_idx = 0;

	// global cell coordinates of the top left of the NxN bounding box
	int x = 7;
	int y = -1;

	sf::Color color;
	sf::Clock timer;

	vector<VI> compute90Rotation(vector<VI> &v);
	void printRotations();
	bool attach();
	bool detectCollision(int x, int y, uint ri);
};