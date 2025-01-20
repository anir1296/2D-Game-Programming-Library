#pragma once

#include "Common.h"

class Ghost {
public:
	Ghost();
	~Ghost();

	static sf::FloatRect getGhostBoundingRect(uint i);
	static void reset();

	void draw(RenderWindow& window);
	void move();
private:
	static std::pair<int, int> getNextCoordinates(uint dir, uint i);
	static bool isGhostInPrison(uint i);

	static Texture *ghost_textures;
	static Sprite ghost_sprites[4];

	static float speed[4];
	static uint dirs[4];
	static float pos[4][2];
	static bool isActive[4];
};
