#include "utils.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>

SDL_Texture* load_texture(const char* file, SDL_Renderer* renderer) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, file);
    if (texture == NULL) {
        log_sdl_error("Failed to load texture");
    }
    return texture;
}

void log_sdl_error(const char* msg) {
    fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
}
