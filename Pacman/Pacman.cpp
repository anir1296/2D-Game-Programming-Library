#include "Pacman.h"
#include "Map.h"
#include "Ghost.h"

// can't make Texture global objects due to an SFML limitation - use pointer
Texture* Pacman::pacmanCloseTexture;
Texture* Pacman::pacmanOpenTexture;

Sprite* Pacman::pacmanSprite;

uint Pacman::score = 0;
int Pacman::lives = 2;

uint Pacman::speed = 4;
uint Pacman::p_dir = 9;

uint Pacman::dist_travelled = 0;

// these correspond to the top left of the pacman sprite, not the origin
uint Pacman::px = 0;
uint Pacman::py = 0;

uint rotationOffset_X = 0;
uint rotationOffset_Y = 0;

float ORIGIN_X = 12.f;
float ORIGIN_Y = 13.f;

uint animate_threshold = 16;

uint pacman_start_cell_x = 10, pacman_start_cell_y = 17;

Pacman::Pacman() {
    pacmanCloseTexture = new Texture();
    pacmanCloseTexture->loadFromFile("sprites/pacman_close.png");

    pacmanOpenTexture = new Texture();
    pacmanOpenTexture->loadFromFile("sprites/pacman_open.png");

    pacmanSprite = new Sprite();
    pacmanSprite->setTexture(*pacmanCloseTexture);

    auto [x, y] = Map::getCoordsForCells(pacman_start_cell_x, pacman_start_cell_y);
    pacmanSprite->setOrigin(Vector2f(ORIGIN_X, ORIGIN_Y));

    setPacmanPos(pacmanSprite, x, y);
}

Pacman::~Pacman() {
}

Texture& Pacman::getPacmanCloseTexture() {
    return *pacmanCloseTexture;
}

Sprite& Pacman::getPacman() {
    return *pacmanSprite;
}

void Pacman::setPacmanPos(Sprite* sprite, int x, int y) {
    auto origin = sprite->getOrigin();

    // px and py represent the top left corner of pacman's sprite
    // because SFML positions based on origin, we have to set the sprite position relative to the origin

    px = x; py = y;
    sprite->setPosition(x + origin.x, y + origin.y);
}

std::pair<int, int> Pacman::getNextCoordinates(uint dir) {
    uint newX = px, newY = py;

    if (dir == 0) {
        newX = px + speed;
    }
    else if (dir == 1) {
        newY = py + speed;
    }
    else if (dir == 2) {
        newX = px - speed;
    }
    else if (dir == 3) {
        newY = py - speed;
    }

    return std::make_pair(newX, newY);
}

bool Pacman::canContinueInDirection(uint dir) {
    auto [newX, newY] = getNextCoordinates(dir);
    return !Map::detectCollision(newX, newY);
}

bool Pacman::detectGhostCollision() {
    auto pac_rect = pacmanSprite->getGlobalBounds();
    float x1_left = pac_rect.left, x1_right = pac_rect.left + pac_rect.width;
    float y1_top = pac_rect.top, y1_bottom = pac_rect.top + pac_rect.height;

    for (int i = 0; i < 4; i++) {
        auto ghost_rect = Ghost::getGhostBoundingRect(i);
        float x2_left = ghost_rect.left, x2_right = ghost_rect.left + ghost_rect.width;
        float max_left = std::max(x1_left, x2_left), min_right = std::min(x1_right, x2_right);

        float y2_top = ghost_rect.top, y2_bottom = ghost_rect.top + ghost_rect.height;
        float max_top = std::max(y1_top, y2_top), min_bottom = std::min(y1_bottom, y2_bottom);

        if (max_left >= min_right || max_top >= min_bottom)
            continue;

        auto delta_x = std::abs(max_left - min_right);
        auto delta_y = std::abs(max_top - min_bottom);

        // this magic check is to give some leniency when trying to avoid ghosts
        if (delta_x > 6 || delta_y > 6)
            return true;
    }

    return false;
}


// 0 - right
// 1 - down
// 2 - left
// 3 - up

void Pacman::setDirection(uint dir) {
    if (!canContinueInDirection(dir)) {
        return;
    }

    if (p_dir != dir) {
        pacmanSprite->setRotation(dir * 90);
        p_dir = dir;
        dist_travelled = 0;
    }
}

bool Pacman::step() {
    if (detectGhostCollision()) {
        return false;
    }

    if (!canContinueInDirection(p_dir)) {
        return true;
    }

    auto [newX, newY] = getNextCoordinates(p_dir);

    if (newX != px || newY != py) {
        dist_travelled += speed;
    }
    setPacmanPos(pacmanSprite, newX, newY);

    auto [col, row] = Map::getCellsForCoords(newX, newY);
    if (DOT_STATUS[(uint)std::floor(row)][(uint)std::floor(col)] == 1) {
        DOT_STATUS[(uint)std::floor(row)][(uint)std::floor(col)] = 0;
        score++;
    }
    animation();

    return true;
}

uint Pacman::getScore() {
    return score*10;
}

void Pacman::decrementLives() {
    lives--;
}

int Pacman::getLives() {
    return lives;
}

void Pacman::draw(RenderWindow& window) {
    window.draw(*pacmanSprite);
}

// create a flashing effect on pacman sprite to identify game over
bool Pacman::flashEffect(RenderWindow& window) {
    static sf::Int32 last_time = 0;
    static sf::Int32 elapsed_time = 0;

    static sf::Int32 last_lives = lives;
    if (lives < last_lives) {
        last_time = 0;
        elapsed_time = 0;
        last_lives = lives;
    }

    static bool draw = true;

    auto time = Map::getElapsedTime().asMilliseconds();
    auto delta = time - last_time;

    if (delta > 200) {
        elapsed_time += 200;
        draw = !draw;
        last_time = time;
    }

    if (elapsed_time > 2000) {
        return true;
    }

    if (draw) {
        window.draw(*pacmanSprite);
    }

    return false;
}

std::pair<float, float> Pacman::getPacmanCell() {
    return Map::getCellsForCoords(px, py);
}

void Pacman::animation() {
    if (dist_travelled == animate_threshold) {
        dist_travelled = 0;
        if (pacmanSprite->getTexture() == pacmanCloseTexture) {
            pacmanSprite->setTexture(*pacmanOpenTexture);
            pacmanSprite->setTextureRect(IntRect(0, 0, 19, 26));
        }
        else {
            pacmanSprite->setTexture(*pacmanCloseTexture);
            pacmanSprite->setTextureRect(IntRect(0, 0, 24, 26));
        }
    }
    return;
}

void Pacman::reset() {
    auto [x, y] = Map::getCoordsForCells(pacman_start_cell_x, pacman_start_cell_y);
    setPacmanPos(pacmanSprite, x, y);
}

