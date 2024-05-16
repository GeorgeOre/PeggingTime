#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include "rendering.h"
#include "controls.h"
#include "physics.h"

// Helper function to initialize SDL and renderer for testing
int init_test_renderer(SDL_Renderer** renderer, SDL_Window** window) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    *window = SDL_CreateWindow("Test Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
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

// Helper function to clean up SDL and renderer
void cleanup_test_renderer(SDL_Renderer* renderer, SDL_Window* window) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void test_render_object() {
    SDL_Renderer* renderer;
    SDL_Window* window;

    if (!init_test_renderer(&renderer, &window)) {
        return;
    }

    Object player = {400.0, 300.0, 0.0, 0.0, 1.0, 1.0};

    // Clear renderer
    clear_renderer(renderer);

    // Render object
    render(&player);

    // Present renderer
    present_renderer(renderer);

    // Simulate a delay to view the rendered output (optional for automated tests)
    SDL_Delay(1000);

    // Clean up
    cleanup_test_renderer(renderer, window);

    printf("test_render_object passed.\n");
}

int main() {
    test_render_object();
    return 0;
}
