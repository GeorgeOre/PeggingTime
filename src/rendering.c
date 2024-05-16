#include "rendering.h"
#include "controls.h"

int init_renderer(SDL_Renderer** renderer, SDL_Window** window) {
    *window = SDL_CreateWindow("PeggingTime", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

void clear_renderer(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void present_renderer(SDL_Renderer* renderer) {
    SDL_RenderPresent(renderer);
}

void render(Object* obj) {
    SDL_Rect fillRect = { (int)obj->x - 10, (int)obj->y - 10, 20, 20 }; // 20x20 rectangle centered on the object's position
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255); // White color
    SDL_RenderFillRect(gRenderer, &fillRect);
}
