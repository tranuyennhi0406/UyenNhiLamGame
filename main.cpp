#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

// Constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 16;
const int MAP_WIDTH = SCREEN_WIDTH / TILE_SIZE;
const int MAP_HEIGHT = SCREEN_HEIGHT / TILE_SIZE;
// Bullet class
class Bullet {
public:
    int x, y;
    int dx, dy;
    SDL_Rect rect;
    bool mutable active = true;

    Bullet(int startX, int startY, int dirX, int dirY) :
        x(startX), y(startY), dx(dirX), dy(dirY), active(true) {
        rect = {x, y, 10, 10};
    }

    void move() {
        x += dx * 5;
        y += dy * 5;
        rect.x = x;
        rect.y = y;

        if (x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT) {
            active = false;
        }
    }

    void render(SDL_Renderer* renderer) {
        if (active) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};

// Wall class
class Wall {
public:
    int x, y;
    SDL_Rect rect;
    bool active;

    Wall(int x, int y) : x(x), y(y), active(true) {
        rect = {x, y, TILE_SIZE, TILE_SIZE};
    }

    void render(SDL_Renderer* renderer) {
        if (active) {
            SDL_SetRenderDrawColor(renderer, 150, 75, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};

// Tank base class
class Tank {
protected:
    int x, y;
    int dirX, dirY;
    int shootDelay;
    SDL_Rect rect;
    vector<Bullet> bullets;

public:
    Tank(int startX, int startY) : x(startX), y(startY), shootDelay(0) {
        rect = {x, y, TILE_SIZE, TILE_SIZE};
    }

    void shoot() {
        if (shootDelay > 0) {
            shootDelay--;
            return;
        }
        shootDelay = 2  ;
        bullets.push_back(Bullet(x + TILE_SIZE/2 - 5, y + TILE_SIZE/2 - 5, dirX, dirY));
    }

    void updateBullets() {
        for (auto &bullet : bullets) {
            bullet.move();
        }
        bullets.erase(remove_if(bullets.begin(), bullets.end(),
                      [](Bullet &b) { return !b.active; }), bullets.end());
    }

    const vector<Bullet>& getBullets() const { return bullets; }
    SDL_Rect getRect() const { return rect; }
};

// PlayerTank class
class PlayerTank : public Tank {
public:
    PlayerTank(int startX, int startY) : Tank(startX, startY) {
        dirX = 0;
        dirY = -1;
    }

    void move(int dx, int dy, const vector<Wall>& walls) {
        if (dx != 0 || dy != 0) {
            dirX = dx;
            dirY = dy;
        }

        int newX = x + dx * 4;
        int newY = y + dy * 4;

        SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};

        for (const auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
                return;
            }
        }

        if (newX >= 0 && newX <= SCREEN_WIDTH - TILE_SIZE &&
            newY >= 0 && newY <= SCREEN_HEIGHT - TILE_SIZE) {
            x = newX;
            y = newY;
            rect.x = x;
            rect.y = y;
        }
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        for (auto &bullet : bullets) {
            bullet.render(renderer);
        }
    }
};

// EnemyTank class
class EnemyTank : public Tank {
    int moveDelay;
    bool active;

public:
    EnemyTank(int startX, int startY) : Tank(startX, startY), active(true) {
        moveDelay = 15;
        dirX = 0;
        dirY = 1;
    }

    void move(const vector<Wall>& walls) {
        if (--moveDelay > 0) return;
        moveDelay = 30;

        int r = rand() % 4;
        if (r == 0) { dirX = 0; dirY = -1; }
        else if (r == 1) { dirX = 0; dirY = 1; }
        else if (r == 2) { dirY = 0; dirX = -1; }
        else if (r == 3) { dirY = 0; dirX = 1; }

        int newX = x + dirX * 2;
        int newY = y + dirY * 2;

        SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};

        for (const auto& wall : walls) {
            if (wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
                dirX = -dirX;
                dirY = -dirY;
                return;
            }
        }

        if (newX >= 0 && newX <= SCREEN_WIDTH - TILE_SIZE &&
            newY >= 0 && newY <= SCREEN_HEIGHT - TILE_SIZE) {
            x = newX;
            y = newY;
            rect.x = x;
            rect.y = y;
        }
    }

    void render(SDL_Renderer* renderer) {
        if (active) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect);

            for (auto &bullet : bullets) {
                bullet.render(renderer);
            }
        }
    }

    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }
};

// Game class
class Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    vector<Wall> walls;
    PlayerTank player;
    int enemyNumber;
    vector<EnemyTank> enemies;
    int score;

public:
    Game() : player((MAP_WIDTH/2) * TILE_SIZE, (MAP_HEIGHT-2) * TILE_SIZE),
             enemyNumber(3), running(true), score(0) {
        if (SDL_Init(SDL_INIT_VIDEO)) {
            cerr << "SDL init failed: " << SDL_GetError() << endl;
            running = false;
            return;
        }

        window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            cerr << "Window creation failed: " << SDL_GetError() << endl;
            running = false;
            return;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            cerr << "Renderer creation failed: " << SDL_GetError() << endl;
            running = false;
            return;
        }

        generateWalls();
        spawnEnemies();
    }

    ~Game() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void generateWalls() {
        walls.clear();
        // Border walls
        for (int i = 0; i < MAP_WIDTH; i++) {
            walls.push_back(Wall(i * TILE_SIZE, 0));
            walls.push_back(Wall(i * TILE_SIZE, (MAP_HEIGHT-1) * TILE_SIZE));
        }
        for (int i = 1; i < MAP_HEIGHT-1; i++) {
            walls.push_back(Wall(0, i * TILE_SIZE));
            walls.push_back(Wall((MAP_WIDTH-1) * TILE_SIZE, i * TILE_SIZE));
        }

        // Random inner walls
        for (int i = 2; i < MAP_HEIGHT-2; i += 2) {
            for (int j = 2; j < MAP_WIDTH-2; j += 2) {
                if (rand() % 3 > 0) {
                    walls.push_back(Wall(j * TILE_SIZE, i * TILE_SIZE));
                }
            }
        }
    }

   void spawnEnemies() {
    enemies.clear();
    for (int i = 0; i < enemyNumber; ++i) {
        int ex, ey;
        bool validPosition = false;

        while (!validPosition) {
            ex = (rand() % (MAP_WIDTH - 4) + 2) * TILE_SIZE;
            ey = (rand() % 5 + 1) * TILE_SIZE;
            validPosition = true;

            SDL_Rect tankRect = {ex, ey, TILE_SIZE, TILE_SIZE};

            // Kiểm tra va chạm với tường
            for (const auto& wall : walls) {
                if (wall.active && SDL_HasIntersection(&tankRect, &wall.rect)) {
                    validPosition = false;
                    break;
                }
            }

            // Kiểm tra va chạm với người chơi
            SDL_Rect playerRect = player.getRect();  // Lưu lại giá trị tránh lỗi
            if (SDL_HasIntersection(&tankRect, &playerRect)) {
                validPosition = false;
            }
        }

        // Thêm EnemyTank vào danh sách
        enemies.push_back(EnemyTank(ex, ey));
    }
}


    void handleEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: player.move(0, -1, walls); break;
                    case SDLK_DOWN: player.move(0, 1, walls); break;
                    case SDLK_LEFT: player.move(-1, 0, walls); break;
                    case SDLK_RIGHT: player.move(1, 0, walls); break;
                    case SDLK_SPACE: player.shoot(); break;
                    case SDLK_ESCAPE: running = false; break;
                }
            }
        }
    }

    void update() {
        player.updateBullets();

        for (auto& enemy : enemies) {
            enemy.move(walls);
            enemy.updateBullets();
            if (rand() % 100 < 3) {
                enemy.shoot();
            }
        }

        checkCollisions();

        enemies.erase(remove_if(enemies.begin(), enemies.end(),
                      [](EnemyTank &e) { return !e.isActive(); }), enemies.end());

        if (enemies.empty()) {
            enemyNumber += 2;
            score += 100;
            spawnEnemies();
        }
    }

   void checkCollisions() {
    // Player bullets vs walls
    auto& bullets = player.getBullets(); // Lấy tham chiếu, tránh copy dữ liệu
    for (auto& bullet : bullets) {
        for (auto& wall : walls) {
                SDL_Rect wallRect=wall.rect;
            if (wall.active && SDL_HasIntersection(&bullet.rect, &wallRect)) {
                wall.active = false;
                bullet.active = false;
                break;
            }
        }
    }

    // Player bullets vs enemies
    auto playerBullets = player.getBullets();
    for (auto& bullet : playerBullets) {
        for (auto& enemy : enemies) {
                SDL_Rect enemyRect=enemy.getRect();
            if (enemy.isActive() && SDL_HasIntersection(&bullet.rect, &enemyRect)) {
                enemy.setActive(false);
                bullet.active = false;
                score += 10;
            }
        }
    }

    // Enemy bullets vs walls
    for (auto& enemy : enemies) {
        auto& enemyBullets = enemy.getBullets();
        for (auto& bullet : enemyBullets) {
            for (auto& wall : walls) {
                if (wall.active && SDL_HasIntersection(&bullet.rect, &wall.rect)) {
                    wall.active = false;
                    bullet.active = false;
                    break;
                }
            }
        }
    }

    // Enemy bullets vs player
    for (auto& enemy : enemies) {
        auto& enemyBullets = enemy.getBullets();
        for (auto& bullet : enemyBullets) {
                SDL_Rect playerRect=player.getRect();
            if (SDL_HasIntersection(&bullet.rect, &playerRect)) {
                running = false;
                std::cout << "Game Over! Score: " << score << std::endl;
                return;
            }
        }
    }
}

    void render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        for (int i = 1; i < MAP_HEIGHT-1; i++) {
            for (int j = 1; j < MAP_WIDTH-1; j++) {
                SDL_Rect tile = {j*TILE_SIZE, i*TILE_SIZE, TILE_SIZE, TILE_SIZE};
                SDL_RenderFillRect(renderer, &tile);
            }
        }

        for (auto& wall : walls) {
            wall.render(renderer);
        }

        player.render(renderer);

        for (auto& enemy : enemies) {
            enemy.render(renderer);
        }

        SDL_RenderPresent(renderer);
    }

    void run() {
        Uint32 lastTime = SDL_GetTicks();
        while (running) {
            Uint32 currentTime = SDL_GetTicks();
            Uint32 deltaTime = currentTime - lastTime;

            if (deltaTime < 16) {
                SDL_Delay(16 - deltaTime);
                continue;
            }

            handleEvents();
            update();
            render();
            lastTime = currentTime;
        }
    }
};

int main(int argc, char* argv[]) {
    srand(time(nullptr));
    Game game;
    game.run();
    return 0;
}
