#pragma once

#include "Common.h"

class Pacman {
public:
	Pacman();
	~Pacman();
	static Sprite& getPacman();
	static Texture& getPacmanCloseTexture();
	static bool step();
	static void setDirection(uint dir);
	static uint getScore();
	static int getLives();
	static void decrementLives();

	static std::pair<float, float> getPacmanCell();
	static void draw(RenderWindow& window);
	static bool flashEffect(RenderWindow& window);

	static void reset();

private:
	static Texture *pacmanCloseTexture;
	static Texture* pacmanOpenTexture;

	static Sprite* pacmanSprite;

	static void setPacmanPos(Sprite* sprite, int x, int y);
	static std::pair<int, int> getNextCoordinates(uint dir);
	static bool canContinueInDirection(uint dir);
	static bool detectGhostCollision();
	static void animation();

	static uint px;
	static uint py;
	static uint p_dir;

	static uint dist_travelled;

	static uint speed;
	static uint score;
	static int lives;
};