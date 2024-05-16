#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>

// Function to load a texture from a file
SDL_Texture* load_texture(const char* file, SDL_Renderer* renderer);

// Function to log an SDL error with a custom message
void log_sdl_error(const char* msg);

#endif // UTILS_H
