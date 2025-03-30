#include "source/config.h"
#include "source/map.h"
#include "source/drawmap.h"
#include "source/bullet.h"
#include "source/loadtexture.h"
#include "source/enemy.h"
#include "source/tank.h"
#include "source/checkbullet.h"
#include "source/checkcollision.h"
#include "source/game.h"
int main() {
    if (!init()) return 1;
    spawnEnemies();
    game();

    return 0;
}

