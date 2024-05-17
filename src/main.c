#include <stdio.h>
#include <stdlib.h>
#include "physics.h"
#include "controls.h"
#include "rendering.h"
#include "game.h"
#include "utils.h"
#include "menu.h"

int main() {
    if (!init_SDL()) {
        return 1;
    }

    if (!init_renderer(&gRenderer, &gWindow)) {
        close_SDL();
        return 1;
    }

    init_menu();

    int running = 1;
    float dt = 0.016; // Assuming 60 FPS

    Game game;
    game.state = GAME_MENU;

    while (running) {
        if (game.state == GAME_MENU) {
            handle_menu_input(&running, &game.state);
            render_menu();
        } else if (game.state == GAME_RUNNING) {
            if (!init_game(&game)) {
                close_SDL();
                return 1;
            }

            while (running && game.state == GAME_RUNNING) {
                handle_input(&game.player, &running, 1);
                update_game(&game, dt);
                render_game(&game);

                SDL_Delay(16); // Roughly 60 FPS

                if (game.state == GAME_OVER) {
                    printf("Game Over!\n");
                    running = 0;
                }
            }

            cleanup_game(&game);
        } else if (game.state == GAME_VERSUS) {
            VersusGame versus_game;
            if (!init_versus_game(&versus_game)) {
                close_SDL();
                return 1;
            }

            while (running && game.state == GAME_VERSUS) {
                handle_input(&versus_game.player1, &running, 1);
                handle_input(&versus_game.player2, &running, 2);

                update_versus_game(&versus_game, dt);
                render_versus_game(&versus_game);

                SDL_Delay(16); // Roughly 60 FPS

                if (versus_game.state == GAME_OVER) {
                    printf("Game Over!\n");
                    running = 0;
                }
            }

            cleanup_versus_game(&versus_game);
        }
    }

    close_SDL();
    return 0;
}
