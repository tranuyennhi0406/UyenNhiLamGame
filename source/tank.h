#ifndef tank_H
#define tank_H
#include "bullet.h"
#include "loadtexture.h"
#include "map.h"
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
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastShotTime >= shootCooldown) {
            bullets.emplace_back(x + TANK_SIZE / 2 - BULLET_SIZE / 2,
                                 y + TANK_SIZE / 2 - BULLET_SIZE / 2,
                                 dirX, dirY);
            lastShotTime = currentTime;
        }
    }

    void draw() {
        SDL_RenderCopy(renderer, directionTexture, NULL, &rect);
    }
};
Tank player(1,1);
#endif
