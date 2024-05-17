#include "menu.h"
#include "game.h"
#include "utils.h"
#include <stdio.h>

static TTF_Font* gFont = NULL;
static MenuItem menuItems[MENU_ITEM_COUNT];
static int selectedItem = 0;

void init_menu() {
    if (TTF_Init() == -1) {
        log_sdl_error("Failed to initialize SDL_ttf");
        exit(1);
    }

    gFont = TTF_OpenFont("assets/fonts/FloryannaDemo-pnlK.ttf", 24);
    if (gFont == NULL) {
        log_sdl_error("Failed to load font");
        exit(1);
    }

    menuItems[0].rect = (SDL_Rect){300, 200, 200, 50};
    menuItems[0].text = "Start Game";
    menuItems[1].rect = (SDL_Rect){300, 300, 200, 50};
    menuItems[1].text = "Quit Game";
}

void render_menu() {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    for (int i = 0; i < MENU_ITEM_COUNT; i++) {
        SDL_Color color = {255, 255, 255, 255};
        if (i == selectedItem) {
            color.r = 255;
            color.g = 0;
            color.b = 0;
        }

        SDL_Surface* surface = TTF_RenderText_Solid(gFont, menuItems[i].text, color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
        SDL_RenderCopy(gRenderer, texture, NULL, &menuItems[i].rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(gRenderer);
}

void handle_menu_input(int* running, GameState* state) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            *running = 0;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedItem--;
                    if (selectedItem < 0) {
                        selectedItem = MENU_ITEM_COUNT - 1;
                    }
                    break;
                case SDLK_DOWN:
                    selectedItem++;
                    if (selectedItem >= MENU_ITEM_COUNT) {
                        selectedItem = 0;
                    }
                    break;
                case SDLK_SPACE:
                    if (selectedItem == 0) {
                        *state = GAME_RUNNING;
                    } else if (selectedItem == 1) {
                        *running = 0;
                    }
                    break;
            }
        } else if (e.type == SDL_MOUSEMOTION) {
            int x = e.motion.x;
            int y = e.motion.y;
            for (int i = 0; i < MENU_ITEM_COUNT; i++) {
                if (x >= menuItems[i].rect.x && x <= menuItems[i].rect.x + menuItems[i].rect.w &&
                    y >= menuItems[i].rect.y && y <= menuItems[i].rect.y + menuItems[i].rect.h) {
                    selectedItem = i;
                }
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            int x = e.button.x;
            int y = e.button.y;
            for (int i = 0; i < MENU_ITEM_COUNT; i++) {
                if (x >= menuItems[i].rect.x && x <= menuItems[i].rect.x + menuItems[i].rect.w &&
                    y >= menuItems[i].rect.y && y <= menuItems[i].rect.y + menuItems[i].rect.h) {
                    if (i == 0) {
                        *state = GAME_RUNNING;
                    } else if (i == 1) {
                        *running = 0;
                    }
                }
            }
        }
    }
}
