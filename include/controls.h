#ifndef CONTROLS_H
#define CONTROLS_H

#include <SDL2/SDL.h>
#include "physics.h"

extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

int init_SDL();
void handle_input(Object* player, int* running, int player_num);
void close_SDL();

#endif // CONTROLS_H
