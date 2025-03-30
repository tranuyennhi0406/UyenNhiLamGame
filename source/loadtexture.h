#ifndef loadtexture_H
#define loadtexture_H
#include "config.h"
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
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    window = SDL_CreateWindow("Tank Battle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;
    IMG_Init(IMG_INIT_PNG);
    heartTexture = IMG_LoadTexture(renderer,"tankheart.png");
    groundTexture = IMG_LoadTexture(renderer,"ground2.png");
    wallTexture = IMG_LoadTexture(renderer,"wall.png");
    steelWallTexture = IMG_LoadTexture(renderer,"steelwall.png");
    enemyTankUpTexture = IMG_LoadTexture(renderer, "enemytankup.png");
    enemyTankDownTexture = IMG_LoadTexture(renderer, "enemytankdown.png");
    enemyTankRightTexture = IMG_LoadTexture(renderer, "enemytankright.png");
    enemyTankLeftTexture = IMG_LoadTexture(renderer, "enemytankleft.png");
    tankUpTexture = IMG_LoadTexture(renderer, "tankerup1.png");
    tankDownTexture = IMG_LoadTexture(renderer, "tankerdown1.png");
    tankLeftTexture = IMG_LoadTexture(renderer, "tankerleft1.png");
    tankRightTexture = IMG_LoadTexture(renderer, "tankerright1.png");
    return tankUpTexture && tankDownTexture && tankLeftTexture && tankRightTexture;
}
#endif
