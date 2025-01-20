#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

typedef unsigned int uint;

constexpr const uint BOARD_WIDTH = 720;
constexpr const uint BOARD_HEIGHT = 960;

constexpr const uint CELL_SIZE = 40;
constexpr const uint NUM_CELLS_HORIZONTAL = BOARD_WIDTH / CELL_SIZE;
constexpr const uint NUM_CELLS_VERTICAL = BOARD_HEIGHT / CELL_SIZE;

const string TITLE = "Tetris!";
char grid[NUM_CELLS_VERTICAL][NUM_CELLS_HORIZONTAL];

constexpr const uint PLAYFIELD_START_COL = 4;
constexpr const uint PLAYFIELD_END_COL = 13;

sf::Color GRAY = Color(128, 128, 128);
sf::Color NAVY = Color(2, 7, 93);

enum CELL_TYPE {
    OUTSIDE = 1, // corresponds to outside the playing field
    FRAME = 2, // corresponds to the border
    EMPTY = 3 // corresponds to an empty cell
};

#pragma region INITIALIZATION
void initializeBoard() {
    // From Tetris fandom wiki, the playfield is 10 blocks wide by 40 blocks tall
    // Tetriminos start falling from row 21 onwards

    // initially all cells are empty
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        for (int j = 0; j < NUM_CELLS_HORIZONTAL; j++) {
            grid[i][j] = 1;
        }
    }

    // set the cells for the frame
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        grid[i][PLAYFIELD_START_COL-1] = 2;
        grid[i][PLAYFIELD_END_COL+1] = 2;
    }

    for (int j = PLAYFIELD_START_COL-1; j <= PLAYFIELD_END_COL+1; j++) {
        grid[0][j] = 2;
        grid[NUM_CELLS_VERTICAL - 1][j] = 2;
    }

    // set the cells for inside the frame
    for (int i = 1; i < NUM_CELLS_VERTICAL-1; i++) {
        for (int j = PLAYFIELD_START_COL; j <= PLAYFIELD_END_COL; j++) {
            grid[i][j] = 3;
        }
    }
}

void initialize() {
    initializeBoard();
}
#pragma endregion

#pragma region RENDERING

void drawBoard(RenderWindow& window) {
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        for (int j = 0; j < NUM_CELLS_HORIZONTAL; j++) {
            
            RectangleShape rect(Vector2f(CELL_SIZE, CELL_SIZE));
            rect.setPosition(Vector2f(j * CELL_SIZE, i * CELL_SIZE));

            sf::Color midnightBlue = Color(25, 25, 112);
            sf::Color c = Color::Black;

            switch (grid[i][j]) {
                case 2: {
                    rect.setOutlineThickness(1);
                    rect.setOutlineColor(midnightBlue);
                    c = GRAY;
                    break;
                }
                case 3: {
                    rect.setOutlineThickness(1);
                    rect.setOutlineColor(midnightBlue);
                    c = NAVY;
                    break;
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

void draw(RenderWindow& window) {
    drawBoard(window);
}

#pragma endregion

#pragma region LOGIC

void tick() {

}

#pragma endregion


int main()
{

    initialize();

    RenderWindow window(sf::VideoMode(BOARD_WIDTH, BOARD_HEIGHT), TITLE);
    window.setFramerateLimit(60);
    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();
        }
        window.clear();
        draw(window);
        window.display();
    }
}
