#include "physics.h"

void apply_gravity(Object* obj, float gravity) {
    obj->vy += gravity;
}

void update_position(Object* obj, float dt) {
    obj->x += obj->vx * dt;
    obj->y += obj->vy * dt;
}

int check_collision(Object* obj1, Object* obj2) {
    // Simple collision detection
    return 0;
}
