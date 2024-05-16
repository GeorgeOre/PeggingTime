#include "ai.h"
#include <math.h>

void init_enemy_ai(EnemyAI* ai, float x, float y, float speed, int attack_range, int attack_power) {
    ai->enemy.x = x;
    ai->enemy.y = y;
    ai->enemy.vx = 0;
    ai->enemy.vy = 0;
    ai->enemy.radius = 10; // example radius
    ai->speed = speed;
    ai->attack_range = attack_range;
    ai->attack_power = attack_power;
}

void update_enemy_ai(EnemyAI* ai, Object* player, float dt) {
    float dx = player->x - ai->enemy.x;
    float dy = player->y - ai->enemy.y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance > ai->attack_range) {
        // Move towards the player
        float angle = atan2(dy, dx);
        ai->enemy.vx = ai->speed * cos(angle);
        ai->enemy.vy = ai->speed * sin(angle);
    } else {
        // Attack player if in range
        ai->enemy.vx = 0;
        ai->enemy.vy = 0;
        // Example attack logic (can be expanded)
        // player->health -= ai->attack_power;
    }

    // Update enemy position based on velocity
    ai->enemy.x += ai->enemy.vx * dt;
    ai->enemy.y += ai->enemy.vy * dt;
}
