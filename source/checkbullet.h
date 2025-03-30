#ifndef checkbullet_H
#define checkbullet_H
#include"bullet.h"
#include"enemy.h"
#include "checkcollision.h"
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
#endif
