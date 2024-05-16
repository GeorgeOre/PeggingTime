#ifndef PHYSICS_H
#define PHYSICS_H

// Struct to represent a physics object
typedef struct {
    float x, y;     // Position
    float vx, vy;   // Velocity
    float mass;     // Mass
    float radius;   // Radius for collision detection
} Object;

// Apply gravity to an object
void apply_gravity(Object* obj, float gravity);

// Update the position of an object based on its velocity and time step
void update_position(Object* obj, float dt);

// Check collision between two objects (simple circle-based collision detection)
int check_collision(const Object* obj1, const Object* obj2);

#endif // PHYSICS_H
