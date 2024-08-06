#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "asset.h"
#include "body.h"
#include "color.h"
#include "polygon.h"
#include "vector.h"

struct body {
  polygon_t *poly;

  double mass;
  vector_t center_of_mass;

  vector_t force;
  vector_t impulse;
  asset_t *speech;
  bool removed;

  void *info;
  free_func_t info_freer;
};

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
  body_t *new_body;
  new_body = body_init_with_info(shape, mass, color, NULL, NULL);
  return new_body;
}

body_t *body_init_with_info(list_t *shape, double mass, rgb_color_t color,
                            void *info, free_func_t info_freer) {
  // Struct init
  body_t *new_body = malloc(sizeof(body_t));
  assert(new_body != NULL);
  // Polygon init
  new_body->poly =
      polygon_init(shape, VEC_ZERO, 0.0, color.r, color.g, color.b);

  // Body params init
  new_body->mass = mass;
  new_body->center_of_mass = VEC_ZERO;
  new_body->force = VEC_ZERO;
  new_body->impulse = VEC_ZERO;
  new_body->speech = NULL;
  new_body->removed = false;
  new_body->info = info;
  new_body->info_freer = info_freer;

  return new_body;
}

void body_free(body_t *body) {
  free_func_t freer = body->info_freer;
  if (body->speech != NULL) {
    asset_destroy(body->speech);
  }
  if (freer != NULL) {
    freer(body->info);
  }
  polygon_free(body->poly);
  free(body);
}
/**
 * Gets the current shape of a body.
 * Returns a newly allocated vector list, which must be list_free()d.
 *
 * @param body a pointer to a body returned from body_init()
 * @return the polygon describing the body's current position
 */
list_t *body_get_shape(body_t *body) {
  list_t *points = polygon_get_points(body->poly);
  list_t *points_copy = list_init(list_size(points), free);
  size_t points_size = list_size(points);
  for (size_t i = 0; i < points_size; i++) {
    vector_t *point = (vector_t *)list_get(points, i);
    vector_t *point_copy = malloc(sizeof(vector_t));
    assert(point_copy != NULL);
    point_copy->x = point->x;
    point_copy->y = point->y;
    list_add(points_copy, point_copy);
  }
  return points_copy;
}

polygon_t *body_get_polygon(body_t *body) { return body->poly; }

void *body_get_info(body_t *body) { return body->info; }

vector_t body_get_velocity(body_t *body) {
  return *polygon_get_velocity(body->poly);
}

void body_set_velocity(body_t *body, vector_t v) {
  polygon_t *body_poly = body->poly;

  polygon_set_velocity(body_poly, v);
}

vector_t body_get_centroid(body_t *body) {
  return polygon_get_center(body->poly);
}

void body_set_centroid(body_t *body, vector_t x) {
  polygon_set_center(body->poly, x);
}

double body_get_rotation(body_t *body) {
  return polygon_get_rotation(body->poly);
}

void body_set_rotation(body_t *body, double angle) {
  polygon_set_rotation(body->poly, angle);
}

rgb_color_t *body_get_color(body_t *body) {
  return polygon_get_color(body->poly);
}

void body_set_color(body_t *body, rgb_color_t *col) {
  polygon_set_color(body->poly, col);
}

void body_set_speech(body_t *body, asset_t *speech) { body->speech = speech; }

void body_reset(body_t *body) {
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}

void body_tick(body_t *body, double dt) {
  vector_t old_v = body_get_velocity(body);

  // Calculate the change in force
  vector_t dF = VEC_ZERO; // dF = I/dt  | F = F0 + I/dt
  if (dt != 0.0) {
    dF = vec_multiply(1 / dt, body->impulse);
  }
  // Update the force
  body->force = vec_add(body->force, dF);

  // If mass is neither 0 or infinity
  if (body->mass != INFINITY && body->mass != 0.0) {
    // Calculate the change in velocity
    vector_t dV = VEC_ZERO; // dv = (F/m)*dt = a*dt
    dV = body->force;

    dV = vec_multiply(dt / body->mass, dV);

    // Update velocity
    body_set_velocity(body, vec_add(body_get_velocity(body), dV));
  }

  // Calculate the translational distance
  vector_t dPOS = VEC_ZERO;
  dPOS = vec_add(body_get_velocity(body), old_v);
  dPOS = vec_multiply(dt / 2, dPOS);

  // Translate the body centroid
  vector_t new_centroid = vec_add(body_get_centroid(body), dPOS);
  body_set_centroid(body, new_centroid);

  // Reset the force and impulse values
  body->force = VEC_ZERO;
  body->impulse = VEC_ZERO;
}

double body_get_mass(body_t *body) { return body->mass; }

/**
 * Adds a force to a running total of magnitude of force acting upon body.
 *
 * @param body a pointer to a body returned from body_init()
 * @param force a vector specifying the force to add
 */
void body_add_force(body_t *body, vector_t force) {
  body->force = vec_add(body->force, force);
}

/**
 * Adds an impulse to a running total of magnitude of impulse acting upon body.
 *
 * @param body a pointer to a body returned from body_init()
 * @param force a vector specifying the impulse to add
 */
void body_add_impulse(body_t *body, vector_t impulse) {
  body->impulse = vec_add(body->impulse, impulse);
}

/**
 * Mark a body to be removed.
 *
 * @param body a pointer to a body returned from body_init()
 */
void body_remove(body_t *body) { body->removed = true; }

/**
 * Return whether or not a body has been marked for removal.
 *
 * @param body a pointer to a body returned from body_init()
 * @return a boolean indicating whether a body is marked for removal
 */
bool body_is_removed(body_t *body) { return (bool)(body->removed == true); }
