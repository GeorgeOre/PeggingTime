#ifndef PHYSICS_H
#define PHYSICS_H

typedef struct {
    float x, y;
    float vx, vy;
    float mass;
} Object;

void apply_gravity(Object* obj, float gravity);
void update_position(Object* obj, float dt);
int check_collision(Object* obj1, Object* obj2);

#endif // PHYSICS_H
