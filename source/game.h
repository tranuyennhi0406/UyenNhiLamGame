#ifndef GAME_H
#define GAME_H
#include "enemy.h"
#include "tank.h"
#include "checkcollision.h"
#include "bullet.h"
#include "map.h"
#include "loadtexture.h"
void game() {
    Tank player(760,560);
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
    SDL_Event event;
event.type = SDL_KEYDOWN;
event.key.keysym.sym = SDLK_DOWN;
SDL_PushEvent(&event);
SDL_Delay(200);
event.type = SDL_KEYUP;
SDL_PushEvent(&event);

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
}

#endif
