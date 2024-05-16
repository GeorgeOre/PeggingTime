#ifndef RENDERING_H
#define RENDERING_H

#include <SDL2/SDL.h>
#include "physics.h"

// Function to render an object
void render(Object* obj);

// Function to initialize the renderer
int init_renderer(SDL_Renderer** renderer, SDL_Window** window);

// Function to clear the renderer
void clear_renderer(SDL_Renderer* renderer);

// Function to present the renderer
void present_renderer(SDL_Renderer* renderer);

#endif // RENDERING_H
