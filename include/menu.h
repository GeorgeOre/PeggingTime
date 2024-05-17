#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "game.h"  // Include game.h to get the definition of GameState

#define MENU_ITEM_COUNT 2

typedef struct {
    SDL_Rect rect;
    char* text;
} MenuItem;

void init_menu();
void render_menu();
void handle_menu_input(int* running, GameState* state);

#endif // MENU_H
