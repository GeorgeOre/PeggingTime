#include "vector.h"
#include <math.h>

const vector_t VEC_ZERO = {0.0, 0.0};

vector_t vec_add(vector_t v1, vector_t v2) {
  return (vector_t){v1.x + v2.x, v1.y + v2.y};
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
  return (vector_t){v1.x - v2.x, v1.y - v2.y};
}

vector_t vec_negate(vector_t v) { return (vector_t){-v.x, -v.y}; }

vector_t vec_multiply(double scalar, vector_t v) {
  return (vector_t){v.x * scalar, v.y * scalar};
}

double vec_dot(vector_t v1, vector_t v2) {
  return (v1.x * v2.x) + (v1.y * v2.y);
}

double vec_cross(vector_t v1, vector_t v2) {
  return (v1.x * v2.y) - (v2.x * v1.y);
}

vector_t vec_rotate(vector_t v, double angle) {
  double rot_x = v.x * cos(angle) - v.y * sin(angle);
  double rot_y = v.x * sin(angle) + v.y * cos(angle);
  return (vector_t){rot_x, rot_y};
}

double vec_get_length(vector_t v) { return sqrt(pow(v.x, 2) + pow(v.y, 2)); }
