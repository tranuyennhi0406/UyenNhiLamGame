#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <math.h>
#include <vector>
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

// Số đỉnh của ngôi sao
const int STAR_POINTS = 5;

// Hàm vẽ ngôi sao
void drawStar(SDL_Renderer* renderer, int x, int y, int radius) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Màu vàng

    SDL_Point points[STAR_POINTS * 2 + 1]; // Mảng lưu điểm
    double angle = -M_PI / 2; // Góc bắt đầu (hướng lên trên)

    for (int i = 0; i < STAR_POINTS * 2; i++) {
        double r = (i % 2 == 0) ? radius : radius / 2.5; // Đỉnh ngoài và trong
        points[i].x = x + r * cos(angle);
        points[i].y = y + r * sin(angle);
        angle += M_PI / STAR_POINTS; // Góc tăng dần
    }

    points[STAR_POINTS * 2] = points[0]; // Đóng kín ngôi sao
    SDL_RenderDrawLines(renderer, points, STAR_POINTS * 2 + 1);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Ngôi sao vàng", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Vẽ hình
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Vẽ nền xanh
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderClear(renderer);

        // Vẽ ngôi sao vàng
        drawStar(renderer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 150);

        SDL_RenderPresent(renderer);
    }

    // Dọn dẹp bộ nhớ
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

