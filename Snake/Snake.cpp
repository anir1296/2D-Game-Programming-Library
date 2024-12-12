#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <ctime>
#include <list>

using namespace sf;
using namespace std;

typedef unsigned int uint;

const string START_TITLE = "Snake! Press Enter to start!";
const string GAME_OVER_TITLE = "Game over! Press Enter to start again!";

string TITLE = START_TITLE;

enum CELL_TYPE {
    EMPTY = 1,
    BORDER = 2,
    SNAKE = 3,
    APPLE = 4
};

enum DIRECTION {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};

// correspond to dx/dy of above enum
uint directions[4][2] = { {0, -1}, {0, 1}, {-1, 0}, { 1, 0 } };

// maintain 4x3 aspect ratio
// width and height should both be divisible by 20
constexpr const uint BOARD_WIDTH = 960;
constexpr const uint BOARD_HEIGHT = 720;

constexpr const uint NUM_CELLS_HORIZONTAL = 96;
constexpr const uint NUM_CELLS_VERTICAL = 72;

constexpr const uint CELL_SIZE = 10;

Color FIELD_COLOR(0x8F, 0xBC, 0x8B);
Color BORDER_COLOR(0x55, 0x6B, 0x2F);
Color SNAKE_COLOR(0x80, 0x00, 0x80);

uint grid[NUM_CELLS_VERTICAL][NUM_CELLS_HORIZONTAL];

Vector2i snakeHeadCell;
uint snakeLength = 3;
DIRECTION snakeDirection;

bool gameOngoing = false;
bool hasProcessedLastMove = false;

std::list<Vector2i> snakeCells;

Vector2i targetCell;

inline void LOOP_GRID(std::function<void(int row, int col)> func) {
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        for (int j = 0; j < NUM_CELLS_HORIZONTAL; j++) {
            func(i, j);
        }
    }
}

inline bool IS_BORDER_CELL(int row, int col) {
    return (row == 0 || row == NUM_CELLS_VERTICAL - 1) || (col == 0 || col == NUM_CELLS_HORIZONTAL - 1);
}

inline bool IS_INVALID_STARTING_CELL(int row, int col) {
    return (row <= 3 || row >= NUM_CELLS_VERTICAL - 3) || (col < 3 || col >= NUM_CELLS_HORIZONTAL - 3);
}

#pragma region INITIALIZATION

void initializeGrid() {

    LOOP_GRID(
        [&](int x, int y) {grid[x][y] = CELL_TYPE::EMPTY; }
    );

    // set the border cells
    for (int i = 0; i < NUM_CELLS_HORIZONTAL; i++) {
        grid[0][i] = CELL_TYPE::BORDER;
        grid[NUM_CELLS_VERTICAL - 1][i] = CELL_TYPE::BORDER;
    }
    for (int i = 0; i < NUM_CELLS_VERTICAL; i++) {
        grid[i][0] = CELL_TYPE::BORDER;
        grid[i][NUM_CELLS_HORIZONTAL - 1] = CELL_TYPE::BORDER;
    }

}

void initializeSnake() {
    // set the snake to any position besides the border cell
    uint snakeCol = std::rand() % NUM_CELLS_HORIZONTAL;
    uint snakeRow = std::rand() % NUM_CELLS_VERTICAL;

    if (IS_BORDER_CELL(snakeRow, snakeCol) || IS_INVALID_STARTING_CELL(snakeRow, snakeCol)) {
        initializeSnake();
        return;
    }

    snakeDirection = static_cast<DIRECTION>(std::rand() % 4);
    snakeHeadCell = Vector2i(snakeCol, snakeRow);

    snakeCells.push_back(snakeHeadCell);

    Vector2i currentCell = snakeHeadCell;
    // grow the snake per the initial direction
    for (int i = 1; i <= snakeLength; i++) {
        Vector2i nextCell;
        int idx = 0;
        switch (snakeDirection) {
        case DIRECTION::UP: {
            idx = 1;
            break;
        }
        case DIRECTION::DOWN: {
            idx = 0;
            break;
        }
        case DIRECTION::LEFT: {
            idx = 3;
            break;
        }
        case DIRECTION::RIGHT: {
            idx = 2;
            break;
        }
        default:
            break;
        }
        nextCell.x = currentCell.x + directions[idx][0];
        nextCell.y = currentCell.y + directions[idx][1];
        snakeCells.push_back(nextCell);
        currentCell = nextCell;
    }
}

void initializeTarget() {
    // set the target to any position besides the border cells and the snake cells
    uint targetCol = std::rand() % NUM_CELLS_HORIZONTAL;
    uint targetRow = std::rand() % NUM_CELLS_VERTICAL;

    if (IS_BORDER_CELL(targetRow, targetCol) || IS_INVALID_STARTING_CELL(targetRow, targetCol)) {
        initializeTarget();
        return;
    }

    bool snakeCell = std::any_of(snakeCells.begin(), snakeCells.end(), [&](Vector2i sc) {
        return (sc.x == targetCol || sc.y == targetRow);
        }
    );

    if (snakeCell) {
        initializeTarget();
        return;
    }

    targetCell = Vector2i(targetCol, targetRow);

}

void initializeGame() {
    std::srand(time(NULL));

    initializeGrid();
    initializeSnake();
    initializeTarget();
}

#pragma endregion

#pragma region RENDERING

void drawGrid(RenderWindow& w) {
    LOOP_GRID(
        [&](int row, int col) {
            RectangleShape rect(Vector2f(CELL_SIZE, CELL_SIZE));
            Color c = IS_BORDER_CELL(row, col) ? BORDER_COLOR : FIELD_COLOR;
            rect.setFillColor(c);
            rect.setPosition(CELL_SIZE * col, CELL_SIZE * row);
            w.draw(rect);
        }
    );
}

void drawSnake(RenderWindow& w) {
    for (auto it = snakeCells.begin(); it != snakeCells.end(); it++) {
        auto& cell = *it;
        RectangleShape rect(Vector2f(CELL_SIZE, CELL_SIZE));
        rect.setFillColor(SNAKE_COLOR);
        rect.setPosition(cell.x * CELL_SIZE, cell.y * CELL_SIZE);
        w.draw(rect);
    }
}

void drawTarget(RenderWindow& w) {
    RectangleShape rect(Vector2f(CELL_SIZE, CELL_SIZE));
    rect.setFillColor(Color::Red);
    rect.setPosition(targetCell.x * CELL_SIZE, targetCell.y * CELL_SIZE);
    w.draw(rect);
}

void render(RenderWindow& w) {
    drawGrid(w);
    drawSnake(w);
    drawTarget(w);
}

#pragma endregion

#pragma region LOGIC

void step() {

    // to move the snake, all we have to do is reset the tail and move the head
    snakeCells.pop_back();

    Vector2i snakeHead = snakeCells.front();
    Vector2i newSnakeHead = snakeHead;
    newSnakeHead.x += directions[snakeDirection][0];
    newSnakeHead.y += directions[snakeDirection][1];

    // NOTE: This should be efficient because STL list insertion at front is O(1)
    snakeCells.insert(snakeCells.begin(), newSnakeHead);

    hasProcessedLastMove = true;

}

void handleMovement(Keyboard::Key k) {
    // validate the move first
    // if we are going up and we get down input, it should be disallowed
    // otherwise, we will run into self collision detection issues

    if (!hasProcessedLastMove)
        return;

    if (k == Keyboard::Key::Up && snakeDirection == DIRECTION::DOWN)
        return;
    if (k == Keyboard::Key::Down && snakeDirection == DIRECTION::UP)
        return;
    if (k == Keyboard::Key::Left && snakeDirection == DIRECTION::RIGHT)
        return;
    if (k == Keyboard::Key::Right && snakeDirection == DIRECTION::LEFT)
        return;

    switch (k) {
    case Keyboard::Key::Up: {
        snakeDirection = DIRECTION::UP;
        break;
    }
    case Keyboard::Key::Down: {
        snakeDirection = DIRECTION::DOWN;
        break;
    }
    case Keyboard::Key::Left: {
        snakeDirection = DIRECTION::LEFT;
        break;
    }
    case Keyboard::Key::Right: {
        snakeDirection = DIRECTION::RIGHT;
        break;
    }
    default:
        break;
    }

    hasProcessedLastMove = false;
}

void growSnake(DIRECTION dir, int length) {
    if (snakeCells.empty()) {
        cerr << "Trying to grow a nonexistent snake!" << endl;
        return;
    }

    Vector2i snakeHead = snakeCells.front();
    Vector2i currentCell = snakeHead;

    int idx = 0;
    switch (snakeDirection) {
    case DIRECTION::UP: {
        idx = 1;
        break;
    }
    case DIRECTION::DOWN: {
        idx = 0;
        break;
    }
    case DIRECTION::LEFT: {
        idx = 3;
        break;
    }
    case DIRECTION::RIGHT: {
        idx = 2;
        break;
    }
    default:
        break;
    }

    for (int i = 1; i <= length; i++) {
        Vector2i nextCell;
        nextCell.x = currentCell.x + directions[idx][0];
        nextCell.y = currentCell.y + directions[idx][1];
        snakeCells.push_back(nextCell);
        currentCell = nextCell;
    }
}

void detectCollision(RenderWindow& window) {
    // we can hit 3 things
    Vector2i snakeHead = snakeCells.front();

    // 1) check if we hit the boundary
    if (IS_BORDER_CELL(snakeHead.y, snakeHead.x)) {
        window.setTitle(GAME_OVER_TITLE);
        gameOngoing = false;
        return;
    }

    // 2) check if we hit (eat) the target (apple) and grow the snek
    if (snakeHead == targetCell) {
        // note that this will grow the snake in the direction of the head, which does not necessarily correspond to the direction of the tail
        // this might make the growth appear janky in some scenarios
        growSnake(snakeDirection, 4);
        initializeTarget();
        return;
    }

    // 3) check if we hit ourselves
    auto it = snakeCells.begin();
    std::advance(it, 1);
    bool selfCollision = std::any_of(it, snakeCells.end(), [&](Vector2i sc) {
        return (sc == snakeHead);
        }
    );

    if (selfCollision) {
        window.setTitle(GAME_OVER_TITLE);
        gameOngoing = false;
        return;
    }
}

void resetGame() {
    snakeCells.clear();
    initializeGame();
}

#pragma endregion

int main()
{
    RenderWindow window(VideoMode(BOARD_WIDTH, BOARD_HEIGHT), TITLE);

    initializeGame();

    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed) {
                Keyboard::Key k = event.key.code;
                if (event.key.code == Keyboard::Enter) {
                    gameOngoing = true;
                    resetGame();
                }
                if (gameOngoing && (k == Keyboard::Up || k == Keyboard::Down || k == Keyboard::Left || k == Keyboard::Right)) {
                    handleMovement(k);
                }
            }
        }
        window.clear(Color::Black);
        // main game loop
        if (gameOngoing) {
            step();
            detectCollision(window);
        }
        // draw here
        render(window);
        window.display();
    }

    return 0;
}
