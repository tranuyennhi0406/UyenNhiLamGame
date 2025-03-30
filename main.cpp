#define SDL_MAIN_HANDLED
#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 40;
const int TANK_SIZE = 40;
const int MOVE_DELAY = 100;
const int BULLET_SPEED = 1;
const int BULLET_SIZE = 10;
int point=0;
int enemiestank=4;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* tankUpTexture = nullptr;
SDL_Texture* tankDownTexture = nullptr;
SDL_Texture* tankLeftTexture = nullptr;
SDL_Texture* tankRightTexture = nullptr;
SDL_Texture* bulletTexture = nullptr;
SDL_Texture* enemyTankUpTexture=nullptr;
SDL_Texture* enemyTankDownTexture=nullptr;
SDL_Texture* enemyTankRightTexture=nullptr;
SDL_Texture* enemyTankLeftTexture=nullptr;
SDL_Texture* groundTexture = nullptr;
SDL_Texture* wallTexture = nullptr;
SDL_Texture* steelWallTexture = nullptr;
SDL_Texture* heartTexture = nullptr;

SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << path << " Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool hasWon = false;
bool running = true;
int brickHealth[15][20] = {0};
int map[15][20] = {
    {0,0,0,0,1,0,0,1,1,0,0,1,1,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0},
    {1,1,0,2,2,2,2,1,0,0,0,0,1,2,2,2,2,0,1,1},
    {0,0,0,2,0,0,0,0,0,2,2,0,0,0,0,0,2,0,0,0},
    {0,1,1,2,1,0,0,1,0,0,0,0,1,0,0,1,2,1,1,0},
    {0,0,0,2,1,0,0,1,0,0,0,0,1,0,0,1,2,0,0,0},
    {1,1,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,0,1,1},
    {0,0,0,0,2,2,2,2,2,0,0,2,2,2,2,2,0,0,0,0},
    {0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0},
    {0,0,0,2,1,0,0,1,0,0,0,0,1,0,0,1,2,0,0,0},
    {1,1,0,2,1,0,0,1,0,0,0,0,1,0,0,1,2,0,1,1},
    {0,0,0,2,0,0,0,0,0,2,2,0,0,0,0,0,2,0,0,0},
    {0,1,1,2,2,2,2,1,0,0,0,0,1,2,2,2,2,1,1,0},
    {0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,1,0,0,1,1,0,0,1,1,0,0,1,0,0,0,0}
};

struct Bullet {
    int x, y, dx, dy;
    SDL_Rect rect;
    bool active;
    int speed = 2;

    Bullet(int startX, int startY, int dirX, int dirY) {
        x = startX;
        y = startY;
        dx = dirX;
        dy = dirY;
        rect = {x, y, BULLET_SIZE, BULLET_SIZE};
        active = true;
    }

    SDL_Rect getRect() const {
        return rect;
    }

    void update() {
        if (active) {
            x += dx * BULLET_SPEED;
            y += dy * BULLET_SPEED;
            rect.x = x;
            rect.y = y;

            int mapX = x / TILE_SIZE;
            int mapY = y / TILE_SIZE;
            if (mapX >= 0 && mapX < 20 && mapY >= 0 && mapY < 15) {
                if (map[mapY][mapX] == 1) {
                    brickHealth[mapY][mapX]++;
                    if (brickHealth[mapY][mapX] >= 5) {
                        map[mapY][mapX] = 0;
                    }
                    active = false;
                } else if (map[mapY][mapX] == 2) {
                    active = false;
                }
            } else {
                active = false;
            }
        }
    }

    void draw() {
        if (active) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};
bool checkCollisionWithWall(const SDL_Rect& bulletRect) {
    int mapX = bulletRect.x / TILE_SIZE;
    int mapY = bulletRect.y / TILE_SIZE;

    if (mapY >= 0 && mapY < 15 && mapX >= 0 && mapX < 20) {
        if (map[mapY][mapX] == 1 || map[mapY][mapX] == 2) {
            return true;
        }
    }
    return false;
}

std::vector<Bullet> bullets;
std::vector<Bullet> enemyBullets;
struct EnemyTank {
    std::vector<Bullet> bullets;
    int x, y;
    int hp;
    int dirX, dirY;
    Uint32 lastShotTime;
    SDL_Rect rect;
    SDL_Texture* directionTexture;
    EnemyTank(int startX, int startY) {
        x = startX;
        y = startY;
        hp = 1;
        dirX = 0;
        dirY = 1;
        lastShotTime = SDL_GetTicks();
        directionTexture = enemyTankDownTexture;
        rect = {x, y, TANK_SIZE, TANK_SIZE};
    }
    void updateBullets() {
    for (auto it = enemyBullets.begin(); it != enemyBullets.end();) {
        it->update();
        if (!it->active) {
            it = enemyBullets.erase(it);
        } else {
            ++it;
        }
    }
}

    void moveRandom() {
        int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
        int choice = rand() % 4;
        int newX = x + directions[choice][0] * TILE_SIZE;
        int newY = y + directions[choice][1] * TILE_SIZE;
        int mapX = newX / TILE_SIZE;
        int mapY = newY / TILE_SIZE;
        if (mapY >= 0 && mapY < 15 && mapX >= 0 && mapX < 20 && map[mapY][mapX] == 0) {
            x = newX;
            y = newY;
            dirX = directions[choice][0];
            dirY = directions[choice][1];
            rect.x = x;
            rect.y = y;
            if (dirX == 0 && dirY == -1) directionTexture = enemyTankUpTexture;
            else if (dirX == 0 && dirY == 1) directionTexture = enemyTankDownTexture;
            else if (dirX == -1 && dirY == 0) directionTexture = enemyTankLeftTexture;
            else if (dirX == 1 && dirY == 0) directionTexture = enemyTankRightTexture;
        }
    }
    void shootIfReady() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShotTime >= 800) {
        enemyBullets.emplace_back(x + TANK_SIZE / 2 - BULLET_SIZE / 2,
                                  y + TANK_SIZE / 2 - BULLET_SIZE / 2,
                                  dirX, dirY);
        lastShotTime = currentTime;
    }
}
    void draw() {
    if (hp > 0) {
        SDL_RenderCopy(renderer, directionTexture, NULL, &rect);
        for (auto& bullet : enemyBullets) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &bullet.rect);
        }
    }
}

};
std::vector<EnemyTank> enemies;
struct Tank {
    int x, y;
    SDL_Rect rect;
    SDL_Texture* directionTexture;
    int dirX, dirY;
    int hp=5;
    void drawHearts() {
    SDL_Rect heartRect = { 10, 10, 20, 20 };
    for (int i = 0; i < hp; ++i) {
        heartRect.x = 10 + i * 25;
        SDL_RenderCopy(renderer, heartTexture, NULL, &heartRect);
    }
}

    Tank(int startX, int startY) {
        x = startX;
        y = startY;
        rect = {x, y, TANK_SIZE, TANK_SIZE};
        directionTexture = tankUpTexture;
        dirX = 0;
        dirY = -1;
    }

    bool canMove(int dx, int dy) {
    int newX = x + dx * TILE_SIZE;
    int newY = y + dy * TILE_SIZE;
    int mapX = newX / TILE_SIZE;
    int mapY = newY / TILE_SIZE;
    if (newX < 0 || newX >= SCREEN_WIDTH || newY < 0 || newY >= SCREEN_HEIGHT) {
    return false;
}
    return (mapY >= 0 && mapY < 15 && mapX >= 0 && mapX < 20 && map[mapY][mapX] == 0);
}

    void move(int dx, int dy, SDL_Texture* newTexture) {
        directionTexture = newTexture;
        dirX = dx;
        dirY = dy;
        if (canMove(dx, dy)) {
            x += dx * TILE_SIZE;
            y += dy * TILE_SIZE;
            rect.x = x;
            rect.y = y;
        }
    }

    void shoot() {
        bullets.emplace_back(x + TANK_SIZE / 2 - BULLET_SIZE / 2, y + TANK_SIZE / 2 - BULLET_SIZE / 2, dirX, dirY);
    }

    void draw() {
        SDL_RenderCopy(renderer, directionTexture, NULL, &rect);
    }
};
Tank player(1,1);
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    window = SDL_CreateWindow("Tank Battle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    IMG_Init(IMG_INIT_PNG);
    heartTexture = IMG_LoadTexture(renderer,"tankheart.png");
    groundTexture = IMG_LoadTexture(renderer,"ground.png");
    wallTexture = IMG_LoadTexture(renderer,"wall.png");
    steelWallTexture = IMG_LoadTexture(renderer,"steelwall.png");
    enemyTankUpTexture = IMG_LoadTexture(renderer, "enemytankup.png");
    enemyTankDownTexture = IMG_LoadTexture(renderer, "enemytankdown.png");
    enemyTankRightTexture = IMG_LoadTexture(renderer, "enemytankright.png");
    enemyTankLeftTexture = IMG_LoadTexture(renderer, "enemytankleft.png");
    tankUpTexture = IMG_LoadTexture(renderer, "tankerup.png");
    tankDownTexture = IMG_LoadTexture(renderer, "tankerdown.png");
    tankLeftTexture = IMG_LoadTexture(renderer, "tankerleft.png");
    tankRightTexture = IMG_LoadTexture(renderer, "tankerright.png");
    return tankUpTexture && tankDownTexture && tankLeftTexture && tankRightTexture;
}
void drawMap(SDL_Renderer* renderer) {
    SDL_Rect tileRect;
    tileRect.w = TILE_SIZE;
    tileRect.h = TILE_SIZE;

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 20; j++) {
            tileRect.x = j * TILE_SIZE;
            tileRect.y = i * TILE_SIZE;

            SDL_Texture* textureToRender = NULL;

            if (map[i][j] == 0) {
                textureToRender = groundTexture;
            } else if (map[i][j] == 1) {
                textureToRender = wallTexture;
            } else if (map[i][j] == 2) {
                textureToRender = steelWallTexture;
            }

            if (textureToRender) {
                SDL_RenderCopy(renderer, textureToRender, NULL, &tileRect);
            }
        }
    }
}

void checkBulletCollisions() {
    for (auto& enemy : enemies) {
        for (auto& bullet : bullets) {
            SDL_Rect bulletRect = bullet.getRect();
                if (bullet.active && SDL_HasIntersection(&bulletRect, &enemy.rect)) {
                bullet.active = false;
                enemy.hp--;
                if(enemy.hp<=0) {
                    point++;
                    enemiestank--;
                }
            }
        }
    }
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                 [](const EnemyTank& e) { return e.hp <= 0; }),
                  enemies.end());
}



void spawnEnemies() {
    enemies.emplace_back(6 * TILE_SIZE, 3 * TILE_SIZE);
    enemies.emplace_back(6 * TILE_SIZE, 11 * TILE_SIZE);
    enemies.emplace_back(13 * TILE_SIZE, 3 * TILE_SIZE);
    enemies.emplace_back(13 * TILE_SIZE, 11 * TILE_SIZE);
}



int main() {
    if (!init()) return 1;
    Tank player(1,1);
    spawnEnemies();
    Uint32 lastMoveTime = 0;

    while (running) {
        if(enemiestank<4) {
                int k=rand()%6;
                if(k==0) {
                enemies.emplace_back(9 * TILE_SIZE, 7 * TILE_SIZE);
                };
                if(k==1) {
                enemies.emplace_back(2 * TILE_SIZE, 1 * TILE_SIZE);
                };
                if(k==2) {
                enemies.emplace_back(2 * TILE_SIZE, 13 * TILE_SIZE);
                };
                if(k==3) {
                enemies.emplace_back(17 * TILE_SIZE, 1 * TILE_SIZE);
                };
                if(k==4) {
                enemies.emplace_back(17 * TILE_SIZE, 13 * TILE_SIZE);
                };
                if(k==5) {
                enemies.emplace_back(10 * TILE_SIZE, 7 * TILE_SIZE);
                };
                enemiestank++;
                };
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                player.shoot();
            }
        }
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastMoveTime >= MOVE_DELAY) {
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            if (keystate[SDL_SCANCODE_LEFT]) player.move(-1, 0, tankLeftTexture);
            else if (keystate[SDL_SCANCODE_RIGHT]) player.move(1, 0, tankRightTexture);
            else if (keystate[SDL_SCANCODE_UP]) player.move(0, -1, tankUpTexture);
            else if (keystate[SDL_SCANCODE_DOWN]) player.move(0, 1, tankDownTexture);
            lastMoveTime = currentTime;
        }
        for (auto it = enemyBullets.begin(); it != enemyBullets.end();) {
    it->update();
    if (checkCollisionWithWall(it->rect)) {
        it = enemyBullets.erase(it);
        continue;
    }
    if (SDL_HasIntersection(&it->rect, &player.rect)) {
        player.hp -= 1;
        if (player.hp==0) {
        std::cout << "                                                   GAME OVER!!!" << std::endl;
        std::cout << "                                              DIEM CUA BAN LA :" << point*1000;
        running = false;
        SDL_Delay(5000);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(0);
    }
        it = enemyBullets.erase(it);
        continue;
    }
    int mapX = it->x / TILE_SIZE;
    int mapY = it->y / TILE_SIZE;
    if (mapY >= 0 && mapY < 15 && mapX >= 0 && mapX < 20) {
        if (map[mapY][mapX] == 1) {
            brickHealth[mapY][mapX]++;
            if (brickHealth[mapY][mapX] >= 5) {
                map[mapY][mapX] = 0;
            }
            it = enemyBullets.erase(it);
            continue;
        } else if (map[mapY][mapX] == 2) {\
            it = enemyBullets.erase(it);
            continue;
        }
    }
    ++it;
}


if (!running) break;
        for (auto& enemy : enemies) {
            if (rand() % 10000 < 4) enemy.moveRandom();
            enemy.shootIfReady();
            enemy.updateBullets();
        }
        for (auto it = bullets.begin(); it != bullets.end();) {
            it->update();
            if (!it->active) it = bullets.erase(it);
            else ++it;
        }
        checkBulletCollisions();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        drawMap(renderer);
        player.draw();
        for (auto& enemy : enemies) enemy.draw();
        for (auto& bullet : bullets) bullet.draw();
        for (auto& bullet : enemyBullets) bullet.draw();
        player.drawHearts();
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(tankDownTexture);
    SDL_DestroyTexture(tankUpTexture);
    SDL_DestroyTexture(tankRightTexture);
    SDL_DestroyTexture(tankLeftTexture);
    SDL_DestroyTexture(enemyTankUpTexture);
    SDL_DestroyTexture(enemyTankDownTexture);
    SDL_DestroyTexture(enemyTankRightTexture);
    SDL_DestroyTexture(enemyTankLeftTexture);
    SDL_DestroyTexture(groundTexture);
    SDL_DestroyTexture(wallTexture);
    SDL_DestroyTexture(steelWallTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
