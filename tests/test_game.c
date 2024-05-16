#include <assert.h>
#include <stdio.h>
#include "game.h"

void test_init_game() {
    Game game;
    int result = init_game(&game);
    assert(result == 1);
    assert(game.player.x == 400.0);
    assert(game.player.y == 300.0);
    assert(game.player.vx == 0.0);
    assert(game.player.vy == 0.0);
    assert(game.num_enemies == NUM_ENEMIES);
    assert(game.state == GAME_RUNNING);
    assert(game.level == 1);
    assert(game.score == 0);
    assert(game.health == 3);
    assert(game.max_health == 3);
    assert(game.lives == NUM_LIVES);
    assert(game.num_balls == 0);
    assert(game.balls == NULL);
    cleanup_game(&game);
}

void test_init_versus_game() {
    VersusGame versus_game;
    int result = init_versus_game(&versus_game);
    assert(result == 1);
    assert(versus_game.player1.x == 200.0);
    assert(versus_game.player1.y == 300.0);
    assert(versus_game.player1.vx == 0.0);
    assert(versus_game.player1.vy == 0.0);
    assert(versus_game.player2.x == 600.0);
    assert(versus_game.player2.y == 300.0);
    assert(versus_game.player2.vx == 0.0);
    assert(versus_game.player2.vy == 0.0);
    assert(versus_game.state == GAME_VERSUS);
    assert(versus_game.score1 == 0);
    assert(versus_game.score2 == 0);
    assert(versus_game.num_balls == 0);
    assert(versus_game.balls == NULL);
    cleanup_versus_game(&versus_game);
}

void test_update_game() {
    Game game;
    init_game(&game);

    // Simulate one update step
    float dt = 0.016;
    update_game(&game, dt);

    // Check that the player has moved due to gravity
    assert(game.player.y > 300.0);

    // Simulate collisions and health reduction
    for (int i = 0; i < game.num_enemies; ++i) {
        game.enemies[i].x = game.player.x;
        game.enemies[i].y = game.player.y;
    }

    update_game(&game, dt);
    assert(game.health < 3);

    cleanup_game(&game);
}

void test_update_versus_game() {
    VersusGame versus_game;
    init_versus_game(&versus_game);

    // Simulate one update step for player 1
    float dt = 0.016;
    update_versus_game(&versus_game, dt);

    // Check that player 1 has moved due to gravity
    assert(versus_game.player1.y > 300.0);

    // Simulate one update step for player 2
    update_versus_game(&versus_game, dt);

    // Check that player 2 has moved due to gravity
    assert(versus_game.player2.y > 300.0);

    cleanup_versus_game(&versus_game);
}

int main() {
    test_init_game();
    test_init_versus_game();
    test_update_game();
    test_update_versus_game();

    printf("All tests passed!\n");
    return 0;
}
