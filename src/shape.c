#include "shape.h"
#include <stdlib.h>
// #include "body.h"

// See whichi one of these defines M_PI idk whats up tbh
#include <math.h>
// #include <SDL2/SDL2_gfxPrimitives.h>
// #include <SDL2/SDL_stdinc.h>

// Triangle Constants
// const double EQUALATERAL_ANGLE = 2*M_PI/3;
// TODO: MAKE THE CONSTANTS MORE ACCURATE PERHAPS
const double EQUALATERAL_ANGLE = 2 * 3.14159265359 / 3;
const double EQUALATERAL_SCALE1 = 0.57735026919; // sqrt(3)/3;
const double EQUALATERAL_SCALE2 = 1 / 2;
const double EQUALATERAL_SCALE3 = 0.288675134595; // sqrt(3)/6;
const vector_t T_345_VEC1 = (vector_t){-1.0 / 6.0, -4.0 / 9.0};
const vector_t T_345_VEC2 = (vector_t){5.0 / 6.0, -4.0 / 9.0};
const vector_t T_345_VEC3 = (vector_t){-1.0 / 6.0, 8.0 / 9.0};

list_t *create_circ_shape(double radius, vector_t center, size_t resolution) {
  list_t *c = list_init(resolution, free);
  for (size_t i = 0; i < resolution; i++) {
    double angle = 2 * M_PI * i / resolution;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){cos(angle) + center.x, sin(angle) + center.y};
    list_add(c, v);
  }

  return c;
}

list_t *create_elipse_shape(double outer_radius, double inner_radius,
                            vector_t center, size_t resolution, double angle) {
  center.y += inner_radius;
  list_t *e = list_init(resolution, free);
  for (size_t i = 0; i < resolution; i++) {
    double e_angle = 2 * M_PI * i / resolution + angle;
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){center.x + inner_radius * cos(e_angle),
                    center.y + outer_radius * sin(e_angle)};
    list_add(e, v);
  }

  return e;
}

list_t *create_square_shape(double width, vector_t center, double angle) {
  list_t *sq = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));
  //   printf("\tINSIDE THE SQUARE CREATOR:\n");
  // printf("\tPoint 1: (%f, %f)\n", center.x + (width / 2 * cos(angle)),
  // center.y + (width / 2 * cos(angle)));
  //   *v = (vector_t){center.x + (width / 2 * cos(angle)),
  //                   center.y + (width / 2 * sin(angle))};
  //   list_add(sq, v);
  //   v = malloc(sizeof(*v));
  //   *v = (vector_t){center.x - (width / 2 * cos(angle)),
  //                   center.y + (width / 2 * sin(angle))};
  //   list_add(sq, v);
  //   v = malloc(sizeof(*v));
  //   *v = (vector_t){center.x - (width / 2 * cos(angle)),
  //                   center.y - (width / 2 * sin(angle))};
  //   list_add(sq, v);
  //   v = malloc(sizeof(*v));
  //   *v = (vector_t){center.x + (width / 2 * cos(angle)),
  //                   center.y - (width / 2 * sin(angle))};
  //   list_add(sq, v);

  *v = (vector_t){center.x + (width / 2 * cos(M_PI / 4.0 + angle)),
                  center.y + (width / 2 * sin(M_PI / 4.0 + angle))};
  list_add(sq, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){center.x + (width / 2 * cos(3 * M_PI / 4.0 + angle)),
                  center.y + (width / 2 * sin(M_PI / 4.0 + angle))};
  list_add(sq, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){center.x + (width / 2 * cos(5 * M_PI / 4.0 + angle)),
                  center.y + (width / 2 * sin(5 * M_PI / 4.0 + angle))};
  list_add(sq, v);
  v = malloc(sizeof(*v));
  *v = (vector_t){center.x + (width / 2 * cos(7 * M_PI / 4.0 + angle)),
                  center.y + (width / 2 * sin(7 * M_PI / 4.0 + angle))};
  list_add(sq, v);

  return sq;
}

list_t *create_rectangle_shape(double width, double height, vector_t center,
                               double angle) {
  list_t *sq = list_init(4, free);
  vector_t *v = malloc(sizeof(*v));

  double half_width = width / 2.0;
  double half_height = height / 2.0;

  *v = (vector_t){
      center.x + (half_width * cos(angle)) - (half_height * sin(angle)),
      center.y + (half_width * sin(angle)) + (half_height * cos(angle))};
  list_add(sq, v);
  v = malloc(sizeof(*v));

  *v = (vector_t){
      center.x - (half_width * cos(angle)) - (half_height * sin(angle)),
      center.y - (half_width * sin(angle)) + (half_height * cos(angle))};
  list_add(sq, v);
  v = malloc(sizeof(*v));

  *v = (vector_t){
      center.x - (half_width * cos(angle)) + (half_height * sin(angle)),
      center.y - (half_width * sin(angle)) - (half_height * cos(angle))};
  list_add(sq, v);
  v = malloc(sizeof(*v));

  *v = (vector_t){
      center.x + (half_width * cos(angle)) + (half_height * sin(angle)),
      center.y + (half_width * sin(angle)) - (half_height * cos(angle))};
  list_add(sq, v);

  return sq;
}

list_t *create_equalateral_triangle_shape(double side, vector_t center,
                                          double angle) {
  list_t *tri = list_init(3, free);
  vector_t *v = malloc(sizeof(*v));

  *v = (vector_t){0, side * EQUALATERAL_SCALE1};
  *v = vec_rotate(*v, angle);
  *v = vec_add(*v, center);
  list_add(tri, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){-side * EQUALATERAL_SCALE2, -side * EQUALATERAL_SCALE3};
  *v = vec_rotate(*v, angle);
  *v = vec_add(*v, center);
  list_add(tri, v);

  v = malloc(sizeof(*v));
  *v = (vector_t){-side * EQUALATERAL_SCALE2, side * EQUALATERAL_SCALE3};
  *v = vec_rotate(*v, angle);
  *v = vec_add(*v, center);
  list_add(tri, v);

  return tri;
}

list_t *create_345_triangle_shape(double side, vector_t center, double angle) {
  list_t *tri = list_init(3, free);
  vector_t *v = malloc(sizeof(*v));

  *v = vec_multiply(side, T_345_VEC1);
  *v = vec_rotate(*v, angle);
  *v = vec_add(*v, center);
  list_add(tri, v);

  v = malloc(sizeof(*v));
  *v = vec_multiply(side, T_345_VEC2);
  *v = vec_rotate(*v, angle);
  *v = vec_add(*v, center);
  list_add(tri, v);

  v = malloc(sizeof(*v));
  *v = vec_multiply(side, T_345_VEC3);
  *v = vec_rotate(*v, angle);
  *v = vec_add(*v, center);
  list_add(tri, v);

  return tri;
}
