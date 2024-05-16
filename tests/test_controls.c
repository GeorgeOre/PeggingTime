#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include "controls.h"
#include "physics.h"

// Helper function to simulate key press event
void simulate_key_press(SDL_Keycode key) {
    SDL_Event event;
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = key;
    SDL_PushEvent(&event);
}

// Helper function to simulate key release event
void simulate_key_release(SDL_Keycode key) {
    SDL_Event event;
    event.type = SDL_KEYUP;
    event.key.keysym.sym = key;
    SDL_PushEvent(&event);
}

void test_handle_input() {
    Object player = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0};
    int running = 1;

    // Simulate pressing the UP key
    simulate_key_press(SDLK_UP);
    handle_input(&player, &running);
    assert(player.vy == -5.0);

    // Simulate releasing the UP key
    simulate_key_release(SDLK_UP);
    handle_input(&player, &running);
    assert(player.vy == 0.0);

    // Simulate pressing the DOWN key
    simulate_key_press(SDLK_DOWN);
    handle_input(&player, &running);
    assert(player.vy == 5.0);

    // Simulate releasing the DOWN key
    simulate_key_release(SDLK_DOWN);
    handle_input(&player, &running);
    assert(player.vy == 0.0);

    // Simulate pressing the LEFT key
    simulate_key_press(SDLK_LEFT);
    handle_input(&player, &running);
    assert(player.vx == -5.0);

    // Simulate releasing the LEFT key
    simulate_key_release(SDLK_LEFT);
    handle_input(&player, &running);
    assert(player.vx == 0.0);

    // Simulate pressing the RIGHT key
    simulate_key_press(SDLK_RIGHT);
    handle_input(&player, &running);
    assert(player.vx == 5.0);

    // Simulate releasing the RIGHT key
    simulate_key_release(SDLK_RIGHT);
    handle_input(&player, &running);
    assert(player.vx == 0.0);

    printf("test_handle_input passed.\n");
}

int main() {
    if (!init_SDL()) {
        return 1;
    }

    test_handle_input();

    close_SDL();
    return 0;
}
