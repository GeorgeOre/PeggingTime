#include <stdio.h>
#include <stdlib.h>
#include "physics.h"
#include "controls.h"
#include "rendering.h"
#include "game.h"

int main() {
    // Game initialization
    Object player = {0, 0, 0, 0, 1.0};
    float gravity = 9.8;
    float dt = 0.016; // Assuming 60 FPS

    while (1) {
        handle_input();
        apply_gravity(&player, gravity);
        update_position(&player, dt);
        render(&player);
    }

    return 0;
}
