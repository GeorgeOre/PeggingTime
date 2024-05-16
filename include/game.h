#ifndef GAME_H
#define GAME_H

#include "physics.h"
#include "controls.h"
#include "rendering.h"
#include "ai.h"

#define NUM_ENEMIES 5
#define NUM_LIVES 3

typedef enum {
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_OVER,
    GAME_VERSUS,
    GAME_MENU
} GameState;

typedef struct {
    Object player;
    EnemyAI enemies[NUM_ENEMIES];
    int num_enemies;
    GameState state;
    int level;
    int score;
    int health;
    int max_health;
    int lives;
    Object* balls; // Projectiles
    int num_balls;
} Game;

typedef struct {
    Object player1;
    Object player2;
    int score1;
    int score2;
    GameState state;
    Object* balls; // Projectiles
    int num_balls;
} VersusGame;

int init_game(Game* game);
int init_versus_game(VersusGame* versus_game);
void update_game(Game* game, float dt);
void update_versus_game(VersusGame* versus_game, float dt);
void render_game(Game* game);
void render_versus_game(VersusGame* versus_game);
void cleanup_game(Game* game);
void cleanup_versus_game(VersusGame* versus_game);

#endif // GAME_H
