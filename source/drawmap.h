#ifndef drawmap_H
#define drawmap_H
#include "config.h"
#include "loadtexture.h"
#include "tank.h"
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

#endif
