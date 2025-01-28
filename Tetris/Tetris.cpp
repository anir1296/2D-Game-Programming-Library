#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Tetrimino.h"
#include "Grid.h"
#include "future"
#include <chrono>
#include <thread>

const string TITLE = "Tetris!";

Tetrimino* spawnShape() {
    const string allowedTypes = "IOTSZJL";
    int idx = std::rand() % 7;

    Tetrimino* t = new Tetrimino(allowedTypes[idx]);
    return t;
}

int main()
{
    Tetrimino::initialize();
    Grid::initialize();

    bool game_ongoing = true;
    Tetrimino* active_block = nullptr;
    sf::Clock block_timer;

    RenderWindow window(sf::VideoMode(BOARD_WIDTH, BOARD_HEIGHT), TITLE);
    window.setFramerateLimit(60);

    bool drop_fast = false;
    bool should_clear = false;

    std::srand(time(nullptr));

    std::future<void> f;

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();

            bool rotation = sf::Keyboard::isKeyPressed(Keyboard::Up);

            if (active_block && e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Left) {
                    if (rotation) {
                        active_block->rotate(false);
                    }
                    else {
                        active_block->move(1);
                    }
                }
                else if (e.key.code == Keyboard::Right) {
                    if (rotation) {
                        active_block->rotate(true);
                    }
                    else {
                        active_block->move(2);
                    }
                }
            }
        }

        drop_fast = sf::Keyboard::isKeyPressed(Keyboard::Down);

        if (!active_block) {
            active_block = spawnShape();
            block_timer.restart();
        }

        window.clear();

        Grid::draw(window);
        active_block->draw(window);

        if (should_clear) {
            f = std::async(std::launch::async, [=]()
                {
                    // super ugly hack to display a flashing effect to clear blocks
                    // there must be a better way to do this
                    std::this_thread::sleep_for(100ms);
                    Grid::dropBlocks();
                });
        }

        should_clear = active_block->drop(drop_fast);

        if (!active_block->isAlive) {
            free(active_block);
            active_block = spawnShape();
        }

        window.display();
    }
}
