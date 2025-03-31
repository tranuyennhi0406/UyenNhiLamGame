#ifndef config_H
#define config_H
#define SDL_MAIN_HANDLED
#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 40;
const int TANK_SIZE = 40;
const int MOVE_DELAY = 100;
const double BULLET_SPEED = 0.2;
const int BULLET_SIZE = 10;
const Uint32 shootCooldown = 600;
Uint32 lastShotTime = 0;
Uint32 lastMoveTime = 0;
int point=0;
int enemiestank=4;
bool hasWon = false;
bool running = true;
int brickHealth[15][20] = {0};
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

#endif
