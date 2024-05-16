#include "game.h"
#include "ai.h"
#include <stdio.h>
#include <stdlib.h>

int init_game(Game* game) {
    game->player.x = 400.0;
    game->player.y = 300.0;
    game->player.vx = 0.0;
    game->player.vy = 0.0;
    game->player.radius = 15.0;
    game->num_enemies = NUM_ENEMIES;
    game->state = GAME_RUNNING;
    game->level = 1;
    game->score = 0;
    game->health = 3;
    game->max_health = 3;
    game->lives = NUM_LIVES;
    game->num_balls = 0;
    game->balls = NULL;

    for (int i = 0; i < NUM_ENEMIES; i++) {
        init_enemy_ai(&game->enemies[i], rand() % 800, rand() % 600, 50.0, 100, 1);
    }

    return 1;
}

int init_versus_game(VersusGame* versus_game) {
    versus_game->player1.x = 200.0;
    versus_game->player1.y = 300.0;
    versus_game->player1.vx = 0.0;
    versus_game->player1.vy = 0.0;
    versus_game->player1.radius = 15.0;

    versus_game->player2.x = 600.0;
    versus_game->player2.y = 300.0;
    versus_game->player2.vx = 0.0;
    versus_game->player2.vy = 0.0;
    versus_game->player2.radius = 15.0;

    versus_game->state = GAME_VERSUS;
    versus_game->score1 = 0;
    versus_game->score2 = 0;
    versus_game->num_balls = 0;
    versus_game->balls = NULL;

    return 1;
}

void update_game(Game* game, float dt) {
    // Update player position based on input
    game->player.x += game->player.vx * dt;
    game->player.y += game->player.vy * dt;

    // Update enemies
    for (int i = 0; i < game->num_enemies; i++) {
        update_enemy_ai(&game->enemies[i], &game->player, dt);
    }

    // Example collision detection and health reduction
    for (int i = 0; i < game->num_enemies; i++) {
        if (check_collision(&game->player, &game->enemies[i].enemy)) {
            game->health -= game->enemies[i].attack_power;
            if (game->health <= 0) {
                game->state = GAME_OVER;
            }
        }
    }
}

void update_versus_game(VersusGame* versus_game, float dt) {
    // Update player 1 position
    versus_game->player1.x += versus_game->player1.vx * dt;
    versus_game->player1.y += versus_game->player1.vy * dt;

    // Update player 2 position
    versus_game->player2.x += versus_game->player2.vx * dt;
    versus_game->player2.y += versus_game->player2.vy * dt;

    // Example collision detection and score update
    if (check_collision(&versus_game->player1, &versus_game->player2)) {
        versus_game->state = GAME_OVER;
        if (versus_game->score1 > versus_game->score2) {
            printf("Player 1 wins!\n");
        } else if (versus_game->score2 > versus_game->score1) {
            printf("Player 2 wins!\n");
        } else {
            printf("It's a tie!\n");
        }
    }
}

void render_game(Game* game) {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    // Render player
    SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
    SDL_Rect playerRect = { game->player.x - game->player.radius, game->player.y - game->player.radius, game->player.radius * 2, game->player.radius * 2 };
    SDL_RenderFillRect(gRenderer, &playerRect);

    // Render enemies
    SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
    for (int i = 0; i < game->num_enemies; i++) {
        SDL_Rect enemyRect = { game->enemies[i].enemy.x - game->enemies[i].enemy.radius, game->enemies[i].enemy.y - game->enemies[i].enemy.radius, game->enemies[i].enemy.radius * 2, game->enemies[i].enemy.radius * 2 };
        SDL_RenderFillRect(gRenderer, &enemyRect);
    }

    SDL_RenderPresent(gRenderer);
}

void render_versus_game(VersusGame* versus_game) {
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    // Render player 1
    SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
    SDL_Rect player1Rect = { versus_game->player1.x - versus_game->player1.radius, versus_game->player1.y - versus_game->player1.radius, versus_game->player1.radius * 2, versus_game->player1.radius * 2 };
    SDL_RenderFillRect(gRenderer, &player1Rect);

    // Render player 2
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
    SDL_Rect player2Rect = { versus_game->player2.x - versus_game->player2.radius, versus_game->player2.y - versus_game->player2.radius, versus_game->player2.radius * 2, versus_game->player2.radius * 2 };
    SDL_RenderFillRect(gRenderer, &player2Rect);

    SDL_RenderPresent(gRenderer);
}

void cleanup_game(Game* game) {
    if (game->balls) {
        free(game->balls);
    }
}

void cleanup_versus_game(VersusGame* versus_game) {
    if (versus_game->balls) {
        free(versus_game->balls);
    }
}
