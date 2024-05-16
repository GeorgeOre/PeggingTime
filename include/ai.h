#ifndef AI_H
#define AI_H

#include "physics.h"

typedef struct {
    Object enemy;
    float speed;
    int attack_range;
    int attack_power;
} EnemyAI;

void init_enemy_ai(EnemyAI* ai, float x, float y, float speed, int attack_range, int attack_power);
void update_enemy_ai(EnemyAI* ai, Object* player, float dt);

#endif // AI_H
