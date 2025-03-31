#ifndef enemy_H
#define enemy_H
#include "bullet.h"
#include "loadtexture.h"
#include "map.h"
#include "drawmap.h"
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
    if (currentTime - lastShotTime >= 1200) {
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
void spawnEnemies() {
    enemies.emplace_back(7 * TILE_SIZE, 3 * TILE_SIZE);
    enemies.emplace_back(6 * TILE_SIZE, 11 * TILE_SIZE);
    enemies.emplace_back(12 * TILE_SIZE, 3 * TILE_SIZE);
    enemies.emplace_back(13 * TILE_SIZE, 11 * TILE_SIZE);
}
#endif
