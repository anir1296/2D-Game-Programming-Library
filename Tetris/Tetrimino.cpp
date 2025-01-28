#include "Tetrimino.h"
#include "Grid.h"

const string allowedTypes = "IOTSZJL";

constexpr const uint fall_time_ms = 500;
constexpr const uint fall_time_ms_fast = 50;

unordered_map<char, sf::Color> Tetrimino::color_mp;

// initial orientations of shapes
uint oi[4][4] =
{	{0, 0, 0, 0},
	{1, 1, 1, 1},
	{0, 0, 0, 0},
	{0, 0, 0, 0}
};

uint oj[4][4] =
{
	{0, 0, 0, 0},
	{1, 1, 1, 0},
	{0, 0, 1, 0},
	{0, 0, 0, 0}
};

uint ol[4][4] =
{
	{0, 0, 0, 0},
	{1, 1, 1, 0},
	{1, 0, 0, 0},
	{0, 0, 0, 0}
};

uint os[4][4] = {
	{0, 0, 0, 0},
	{0, 1, 1, 0},
	{1, 1, 0, 0},
	{0, 0, 0, 0}
};

uint ot[4][4] = {
	{0, 0, 0, 0},
	{1, 1, 1, 0},
	{0, 1, 0, 0},
	{0, 0, 0, 0}
};

uint oz[4][4] = {
	{0, 0, 0, 0},
	{1, 1, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 0, 0}
};

uint oo[4][4] =
{	{0, 0, 0, 0},
	{0, 1, 1, 0},
	{0, 1, 1, 0},
	{0, 0, 0, 0}
};

void Tetrimino::initialize() {
	color_mp['I'] = sf::Color::Cyan;
	color_mp['J'] = sf::Color::Blue;
	color_mp['L'] = sf::Color(255, 172, 28); // orange
	color_mp['O'] = sf::Color::Yellow;
	color_mp['S'] = sf::Color::Green;
	color_mp['T'] = sf::Color(160, 32, 240); //purple
	color_mp['Z'] = sf::Color::Red;
}

Tetrimino::Tetrimino(char type) {
	if (allowedTypes.find(type) == string::npos) {
		cerr << "Invalid tetrimino type!" << endl;
		return;
	}
	this->type = type;
	this->color = color_mp[this->type];

	uint(*orientation)[4] = oi;
	switch (this->type) {
	case 'I': {
		orientation = oi;
		NUM_ROTATIONS = 2;
		N = 4;
		break;
	}
	case 'O': {
		orientation = oo;
		NUM_ROTATIONS = 1;
		N = 4;
		break;
	}
	case 'T': {
		orientation = ot;
		NUM_ROTATIONS = 4;
		N = 3;
		break;
	}
	case 'S': {
		orientation = os;
		NUM_ROTATIONS = 4;
		N = 3;
		break;
	}
	case 'Z': {
		orientation = oz;
		NUM_ROTATIONS = 4;
		N = 3;
		break;
	}
	case 'J': {
		orientation = oj;
		NUM_ROTATIONS = 4;
		N = 3;
		break;
	}
	case 'L': {
		orientation = ol;
		NUM_ROTATIONS = 4;
		N = 3;
	}
	default:
		break;
	}

	this->rotations.push_back(vector<VI>(4, VI(N, 0)));

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			rotations[0][i][j] = orientation[i][j];
		}
	}

	for (int i = 1; i < NUM_ROTATIONS; i++) {
		rotations.emplace_back(compute90Rotation(rotations[i - 1]));
	}

	timer.restart();
	//printRotations();
}

// perform a 90 degree rotation
// compute the transpose of the matrix and then reverse each row (thanks Leetcode)
vector<VI> Tetrimino::compute90Rotation(vector<VI> &v) {
	vector<VI> r(4, VI(N, 0));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			r[i][j] = v[i][j];
		}
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j <= i; j++) {
			std::swap(r[i][j], r[j][i]);
		}
	}

	for (int i = 0; i < N; i++) {
		std::reverse(r[i].begin(), r[i].end());
	}

	return r;
}

void Tetrimino::printRotations() {
	for (int i = 0; i < NUM_ROTATIONS; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				std::cout << rotations[i][j][k] << " ";
			}
			std::cout << endl;
		}
		std::cout << endl << endl;;
	}
}

// down = 0
// left = 1
// right = 2
bool Tetrimino::move(uint dir) {
	uint dx = 0,  dy = 0;
	if (dir == 0) {
		dy = 1;
	} 
	else if (dir == 1) {
		dx = -1;
	}
	else if (dir == 2) {
		dx = 1;
	}

	int x = this->x + dx, y = this->y + dy;

	// collision detection
	// 1. we hit the walls
	// 2. we hit the floor OR an already placed block

	if (detectCollision(x, y, this->rot_idx)) {
		if (dir == 0) {
			return attach();
		}
		return false;
	}
	this->x = x; this->y = y;

	return false;
}

bool Tetrimino::detectCollision(int x, int y, uint ri) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (rotations[ri][i][j] == 1) {
				int cell_x = x + j, cell_y = y + i;

				if (Grid::isCellOccuplied(cell_x, cell_y)) {
					return true;;
				}
			}
		}
	}

	return false;
}

bool Tetrimino::attach() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (rotations[rot_idx][i][j] == 1) {
				int cell_x = x + j, cell_y = y + i;
				grid[cell_y][cell_x] = type;
			}
		}
	}

	this->isAlive = false;

	bool did_clear = false;
	for (int i = 0; i < N; i++) {
		int cell_y = y + i;
		if (y <= PLAYFIELD_END_ROW) {
			did_clear |= Grid::clearLine(cell_y);
		}
	}

	return did_clear;
}

bool Tetrimino::drop(bool fast) {
	uint threshold = fast ? fall_time_ms_fast : fall_time_ms;
	static uint last_time = 0;
	uint elapsedTime = timer.getElapsedTime().asMilliseconds();
	if (last_time == 0) {
		last_time = elapsedTime;
	}

	bool res = false;
	if (elapsedTime > last_time + threshold) {
		res = this->move(0);
		last_time = 0;
		timer.restart();
	}
	return res;
}

void Tetrimino::rotate(bool right) {
	uint ri = this->rot_idx;
	if (right) {
		ri = (++ri) % NUM_ROTATIONS;
	}
	else {
		ri = (ri == 0) ? NUM_ROTATIONS - 1 : ri - 1;
	}

	if (detectCollision(this->x, this->y, ri)) {
		return;
	}

	this->rot_idx = ri;
}

void Tetrimino::draw(RenderWindow& w) {
	sf::Color borderColor = sf::Color(112, 128, 144); // slate gray

	int n = this->N, x = this->x, y = this->y;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (rotations[rot_idx][i][j] == 1) {
				RectangleShape rect(Vector2f(CELL_SIZE-1, CELL_SIZE-1));
				rect.setFillColor(this->color);
				rect.setOutlineColor(borderColor);
				rect.setOutlineThickness(2.0f);

				int x2 = x + j, y2 = y + i;
				int x_pos = x2 * CELL_SIZE, y_pos = y2 * CELL_SIZE;
				rect.setPosition(x_pos, y_pos);
				w.draw(rect);
			}
		}
	}

}