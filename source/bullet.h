#ifndef bullet_H
#define bullet_H
#include"config.h"
#include"loadtexture.h"
#include "map.h"
struct Bullet {
    double x, y, dx, dy;
    SDL_Rect rect;
    bool active;

    Bullet(double startX, double startY, double dirX, double dirY) {
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
            SDL_SetRenderDrawColor(renderer, 0, 0, 139, 0);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
};
std::vector<Bullet> bullets;
std::vector<Bullet> enemyBullets;
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

#endif
