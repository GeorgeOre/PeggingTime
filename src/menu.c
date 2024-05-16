#include <SDL2/SDL.h>
#include <stdio.h>
#include "game.h"
#include "controls.h"
#include "rendering.h"

void render_menu() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    // Render menu options
    // This is a placeholder; ideally, you would render text or images
    SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
    SDL_Rect playRect = { 300, 200, 200, 50 };
    SDL_RenderFillRect(gRenderer, &playRect);

    SDL_Rect quitRect = { 300, 300, 200, 50 };
    SDL_RenderFillRect(gRenderer, &quitRect);

    SDL_RenderPresent(gRenderer);
}

void handle_menu_input(int* running, GameState* state) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            *running = 0;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_RETURN: // Enter key to start the game
                    *state = GAME_RUNNING;
                    break;
                case SDLK_q: // Q key to quit the game
                    *running = 0;
                    break;
            }
        }
    }
}
