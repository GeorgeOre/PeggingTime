#include <stdio.h>
#include <assert.h>
#include "physics.h"

// Helper function to compare floating point numbers
int float_equal(float a, float b, float epsilon) {
    return fabs(a - b) < epsilon;
}

void test_apply_gravity() {
    Object obj = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0};
    float gravity = 9.8;
    apply_gravity(&obj, gravity);

    assert(float_equal(obj.vy, gravity, 0.0001));
    printf("test_apply_gravity passed.\n");
}

void test_update_position() {
    Object obj = {0.0, 0.0, 10.0, 5.0, 1.0, 1.0};
    float dt = 1.0;
    update_position(&obj, dt);

    assert(float_equal(obj.x, 10.0, 0.0001));
    assert(float_equal(obj.y, 5.0, 0.0001));
    printf("test_update_position passed.\n");
}

void test_check_collision() {
    Object obj1 = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0};
    Object obj2 = {1.5, 0.0, 0.0, 0.0, 1.0, 1.0};
    Object obj3 = {3.0, 0.0, 0.0, 0.0, 1.0, 1.0};

    assert(check_collision(&obj1, &obj2) == 1);
    assert(check_collision(&obj1, &obj3) == 0);
    printf("test_check_collision passed.\n");
}

int main() {
    test_apply_gravity();
    test_update_position();
    test_check_collision();

    printf("All tests passed.\n");
    return 0;
}
