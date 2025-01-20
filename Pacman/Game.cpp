#include "Map.h"
#include "Pacman.h"
#include "Ghost.h"

const string GAME_TITLE = "Pacman";

constexpr const uint WINDOW_WIDTH = 600;
constexpr const uint WINDOW_HEIGHT = 700;

Sprite life_sprites[2];

void drawBoardExtras(RenderWindow& window, Text &gameScore, Text &gameOverText, uint score, int lives, bool gameOver) {
    string score_str = "High Score: " + std::to_string(score);
    gameScore.setString(score_str);

    window.draw(gameScore);

    for (int i = 0; i < lives; i++) {
        life_sprites[i].setPosition(BOARD_OFFSET_LEFT + i * 30, 650);
        window.draw(life_sprites[i]);
    }

    if (gameOver) {
        gameOverText.setString("Game Over.");
        window.draw(gameOverText);
    }
}

int main()
{
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), GAME_TITLE);
    window.setFramerateLimit(40);

    std::srand(std::time(0));

    Pacman *p = new Pacman();
    Ghost* g = new Ghost();
    Map::initialize();

    sf::Font gameFont;
    if (!gameFont.loadFromFile("Emulogic.ttf")) {
        return 0;
    }

    sf::Text gameScoreText;
    gameScoreText.setCharacterSize(14);
    gameScoreText.setFont(gameFont);
    gameScoreText.setPosition(WINDOW_WIDTH/2 - 100, 40);

    sf::Text gameOverText;
    gameOverText.setCharacterSize(10);
    gameOverText.setFont(gameFont);
    gameOverText.setFillColor(Color::Yellow);
    gameOverText.setPosition(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT - 40);

    Texture pacTexture = Pacman::getPacmanCloseTexture();
    life_sprites[0] = Sprite(pacTexture); life_sprites[1] = Sprite(pacTexture);

    bool game_ongoing = true;
    bool game_over = false;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(Keyboard::Right)) {
            Pacman::setDirection(0);
        }
        else if (sf::Keyboard::isKeyPressed(Keyboard::Left)) {
            Pacman::setDirection(2);
        }
        else if (sf::Keyboard::isKeyPressed(Keyboard::Down)) {
            Pacman::setDirection(1);
        }
        else if (sf::Keyboard::isKeyPressed(Keyboard::Up)) {
            Pacman::setDirection(3);
        }

        window.clear();

        Map::drawWalls(window);
        Map::drawInnerWalls(window);
        Map::drawDots(window);

        drawBoardExtras(window, gameScoreText, gameOverText, Pacman::getScore(), Pacman::getLives(), game_over);

        if (game_ongoing && !Pacman::step()) {
            game_ongoing = false;
        }

        if (game_ongoing && !game_over) {
            Pacman::draw(window);
            g->move();
        }
        else {
            bool restart = Pacman::flashEffect(window);
            if (restart) {
                if (Pacman::getLives() > 0) {
                    Pacman::decrementLives();
                    game_ongoing = true;
                    Map::reset();
                    Pacman::reset();
                    Ghost::reset();
                }
                else {
                    game_over = true;
                    game_ongoing = false;
                }
            }
        }

        g->draw(window);

        window.display();
    }

    delete p;
    delete g;
}
