#include "controls.h"
#include <stdio.h>

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

int init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

void handle_input(Object* player, int* running, int player_num) {
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            *running = 0;
        } else if (e.type == SDL_KEYDOWN) {
            if (player_num == 1) {
                // Player 1 controls (Arrow keys)
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        player->vy = -50.0;
                        break;
                    case SDLK_DOWN:
                        player->vy = 50.0;
                        break;
                    case SDLK_LEFT:
                        player->vx = -50.0;
                        break;
                    case SDLK_RIGHT:
                        player->vx = 50.0;
                        break;
                    case SDLK_SPACE:
                        // Implement shooting or other action
                        break;
                }
            } else if (player_num == 2) {
                // Player 2 controls (WASD keys)
                switch (e.key.keysym.sym) {
                    case SDLK_w:
                        player->vy = -50.0;
                        break;
                    case SDLK_s:
                        player->vy = 50.0;
                        break;
                    case SDLK_a:
                        player->vx = -50.0;
                        break;
                    case SDLK_d:
                        player->vx = 50.0;
                        break;
                }
            }
        } else if (e.type == SDL_KEYUP) {
            if (player_num == 1) {
                // Player 1 controls (Arrow keys)
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                    case SDLK_DOWN:
                        player->vy = 0.0;
                        break;
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        player->vx = 0.0;
                        break;
                }
            } else if (player_num == 2) {
                // Player 2 controls (WASD keys)
                switch (e.key.keysym.sym) {
                    case SDLK_w:
                    case SDLK_s:
                        player->vy = 0.0;
                        break;
                    case SDLK_a:
                    case SDLK_d:
                        player->vx = 0.0;
                        break;
                }
            }
        }
    }
}

void close_SDL() {
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    SDL_Quit();
}
