#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include "./Connector.hpp"

using namespace sf;

struct Piece {
    Sprite s;
    bool isDead;
};

// note: this is double the h/w of the board image
// they are both the same value, so it is only for readability that there are two variables
constexpr const unsigned int BOARD_WIDTH = 1008;
constexpr const unsigned int BOARD_HEIGHT = 1008;

constexpr const unsigned int BOARD_BORDER = 48;

constexpr const int ADJUSTED_BOARD_WIDTH = BOARD_WIDTH - 2 * BOARD_BORDER;
constexpr const int ADJUSTED_BOARD_HEIGHT = BOARD_HEIGHT - 2 * BOARD_BORDER;

constexpr const int CELL_HEIGHT = ADJUSTED_BOARD_HEIGHT / 8;
constexpr const int CELL_WIDTH = ADJUSTED_BOARD_HEIGHT / 8;

constexpr const unsigned int FIGURES_IMAGE_WIDTH = 340;
constexpr const unsigned int FIGURES_IMAGE_HEIGHT = 120;

constexpr const float SCALE = 2.0f;
const Vector2f SCALE_V = Vector2f(SCALE, SCALE);

Piece white[16], black[16];

/**
 * Given 8x8 grid coordinates and a sprite, position piece in the grid
 */
void positionPieceInGrid(Sprite& s, int i, int j) {
    int x = ADJUSTED_BOARD_WIDTH / 8 * i + BOARD_BORDER;
    int y = ADJUSTED_BOARD_WIDTH / 8 * j + BOARD_BORDER;

    if (j > 3) {
        y -= j; // magic offset just to make look better (sprite dimensions not entirely exact in code)
    }
    s.setPosition(x, y);
}

/**
 *  Given a sprite, get the position of the piece in the 8x8 grid
 */
std::pair<int, int> getGridPositionPiece(Sprite& s) {
    auto pos = s.getPosition();

    return std::make_pair(1, 1);
}

/**
 * Given screen coordinates, convert to 8x8 grid coords
 */
std::pair<int, int> convertScreenCoordinatesToGrid(int x, int y) {
    int top_left_board_x = BOARD_BORDER;
    int top_left_board_y = BOARD_BORDER;

    if (x < top_left_board_x || y < top_left_board_y) {
        return std::make_pair(-1, -1);
    }

    x -= top_left_board_x;
    y -= top_left_board_y;

    return std::make_pair(x / CELL_WIDTH, y / CELL_HEIGHT);
}

/**
* Given 8x8 grid coordinates, return the chess representation of the square as a string
* note: origin of the grid (a,0) is at top left, unlike the normal chess board syntax
*/
std::string convertGridCoordinatesToFen(int x, int y) {
    char first = 'a' + x;
    char second = '8' - y;
    return std::string() + first + second;
}

/**
 * Given chess representation of square, get the 8x8 coordinates
 * note: stockfish returns first row as 1, not 0
 */
std::pair<int, int> convertFenToGridCoordinates(std::string& fen) {
    int x = fen[0] - 'a';
    int y = '8' - fen[1];

    return std::make_pair(x, y);
}

std::pair<int, int> convertFenToPositionCoordinates(std::string &fen) {
    auto p = convertFenToGridCoordinates(fen);

    int pos_x = BOARD_BORDER;
    int pos_y = BOARD_BORDER;

    pos_x += (CELL_WIDTH * p.first);
    pos_y += (CELL_HEIGHT * p.second);

    return std::make_pair(pos_x, pos_y);
}

void LoadAndPositionInitialPieces(Texture &piecesTexture) {
    int FIGURE_WIDTH = FIGURES_IMAGE_WIDTH / 6;
    int FIGURE_HEIGHT = FIGURES_IMAGE_HEIGHT / 2 - 3;

    // position bottom pieces (non-pawns)
    // black
    for (int i = 0; i < 8; i++) {
        int j = (i > 4) ? 2 - (i - 5) : i;
        int x_topleft_image = FIGURE_WIDTH * j;
        int y_topLeft_image = 0;
        black[i].s = Sprite(piecesTexture, IntRect(x_topleft_image, y_topLeft_image, FIGURE_WIDTH, FIGURE_HEIGHT));
        black[i].s.setScale(SCALE_V);

        positionPieceInGrid(black[i].s, i, 0);
    }

    // white
    for (int i = 0; i < 8; i++) {
        int j = (i > 4) ? 2 - (i - 5) : i;
        int x_topleft_image = FIGURE_WIDTH * j;
        int y_topLeft_image = FIGURES_IMAGE_HEIGHT / 2;
        white[i].s = Sprite(piecesTexture, IntRect(x_topleft_image, y_topLeft_image, FIGURE_WIDTH, FIGURE_HEIGHT));
        white[i].s.setScale(SCALE_V);

        positionPieceInGrid(white[i].s, i, 7);
    }

    int x_pawn_image = FIGURE_WIDTH * 5;

    // position top pieces (pawns)
    for (int i = 8; i <= 15; i++) {
        black[i].s = Sprite(piecesTexture, IntRect(x_pawn_image, 0, FIGURE_WIDTH, FIGURE_HEIGHT));
        black[i].s.setScale(SCALE_V);
        white[i].s = Sprite(piecesTexture, IntRect(x_pawn_image, FIGURE_HEIGHT, FIGURE_WIDTH, FIGURE_HEIGHT));
        white[i].s.setScale(SCALE_V);

        positionPieceInGrid(black[i].s, (i - 8), 1);
        positionPieceInGrid(white[i].s, (i - 8), 6);
    }
}

int main()
{
    RenderWindow window(VideoMode(BOARD_WIDTH, BOARD_HEIGHT), "Chess! Play as white!", Style::Default);
    Texture boardTexture;
    boardTexture.loadFromFile("board.png");

    // load the textures

    Sprite boardSprite;
    boardSprite.setTexture(boardTexture);
    boardSprite.setScale(SCALE_V);

    Texture piecesTexture;
    piecesTexture.loadFromFile("figures.png");

    LoadAndPositionInitialPieces(piecesTexture);

    wchar_t stockFishpath[] = L"stockfish";
    ConnectToEngine(stockFishpath);

    std::string fen = "";

    // state of piece being moved
    int p = -1;
    int px, py;
    float dx, dy;

    bool white_move = true;
    int k = 0;
    std::string move = "";

    // main game loop
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            else if (white_move && event.type == Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                // save the piece info
                px = event.mouseButton.x;
                py = event.mouseButton.y;

                for (int i = 0; i < 16; i++) {
                    if (!white[i].isDead && white[i].s.getGlobalBounds().contains(px, py)) {
                        p = i;
                        auto grid_pos = convertScreenCoordinatesToGrid(px, py);

                        move += convertGridCoordinatesToFen(grid_pos.first, grid_pos.second);
                    }
                }
            }
            else if (white_move && event.type == Event::MouseMoved) {
                // move the piece according to the delta
                int x = event.mouseMove.x;
                int y = event.mouseMove.y;

                if (p >= 0) {
                    auto& s = white[p].s;
                    auto& pos = s.getPosition();
                    dx = x-px, dy = y-py;
                    s.move(dx, dy);

                    px = x;
                    py = y;
                }
            }
            else if (white_move && event.type == Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                // snap to the grid and capture piece if possible
                int x = event.mouseButton.x;
                int y = event.mouseButton.y;

                if (p >= 0) {
                    auto grid_pos = convertScreenCoordinatesToGrid(x, y);

                    if (grid_pos.first != -1) {
                        auto& s = white[p].s;
                        // todo: need to check if this is square of same color piece or if it is the same original pos
                        positionPieceInGrid(s, grid_pos.first, grid_pos.second);

                        // check if we captured any pieces
                        for (int i = 0; i < 16; i++) {
                            if (!black[i].isDead && black[i].s.getGlobalBounds().contains(x, y)) {
                                black[i].isDead = true;
                            }
                        }

                        p = -1;
                        white_move = false;
                        move += convertGridCoordinatesToFen(grid_pos.first, grid_pos.second) + " ";
                        fen += move;
                        std::cout << move << std::endl;
                        move = "";
                    }
                }
            }
        }

        if (!white_move) {
            // get the optimal move from stockfish
            if (p == -1) {
                std::string move = getNextMove(fen);

                if (move.length() != 4) {
                    std::cerr << "Bad move from stockfish: " << move << std::endl;
                    continue;
                }

                std::string piece_str = move.substr(0, 2);
                auto grid_pos = convertFenToGridCoordinates(piece_str);
                for (int i = 0; i < 16; i++) {
                    if (!black[i].isDead) {
                        auto pos = black[i].s.getPosition();
                        auto grid_pos_piece = convertScreenCoordinatesToGrid(pos.x, pos.y);

                        if (grid_pos == grid_pos_piece) {
                            p = i;
                            break;
                        }
                    }
                }

                fen += (move + " ");
                std::cout << move << std::endl;

                std::string move_str = move.substr(2, 2);
                auto move_pos = convertFenToPositionCoordinates(move_str);
                px = move_pos.first, py = move_pos.second;

                auto& s = black[p].s;
                auto& pos = s.getPosition();
                dx = px - pos.x, dy = py - pos.y;
                dx /= 10000; dy /= 10000;
            } else {
                // simulate the movement of a piece
                if (k < 10000) {
                    auto& s = black[p].s;
                    s.move(dx, dy);
                    k++;
                }
                else {
                    auto& s = black[p].s;
                    auto pos = s.getPosition();
                    // float calculations from simulation will make pos slightly less than it should be
                    pos.x = std::ceil(pos.x); pos.y = std::ceil(pos.y);
                    s.setPosition(pos.x, pos.y);
                    int x = pos.x, y = pos.y;

                    // check if enemy captured any pieces
                    for (int i = 0; i < 16; i++) {
                        if (!white[i].isDead && white[i].s.getGlobalBounds().contains(x, y)) {
                            white[i].isDead = true;
                        }
                    }

                    k = 0;
                    white_move = true;
                    p = -1;
                }
            }

        }

        window.clear();
        window.draw(boardSprite);
        for (int i = 0; i < 16; i++) {
            if (!black[i].isDead) window.draw(black[i].s);
            if (!white[i].isDead) window.draw(white[i].s);
        }
        window.display();
    }

    CloseConnection();
    return 0;
}
