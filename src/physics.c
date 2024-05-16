#include "physics.h"
#include <math.h>

// Apply gravity to an object
void apply_gravity(Object* obj, float gravity) {
    obj->vy += gravity;
}

// Update the position of an object based on its velocity and time step
void update_position(Object* obj, float dt) {
    obj->x += obj->vx * dt;
    obj->y += obj->vy * dt;
}

// Check collision between two objects (simple circle-based collision detection)
int check_collision(const Object* obj1, const Object* obj2) {
    float dx = obj1->x - obj2->x;
    float dy = obj1->y - obj2->y;
    float distance = sqrtf(dx * dx + dy * dy);
    return distance < (obj1->radius + obj2->radius);
}
