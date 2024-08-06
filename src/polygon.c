#include "polygon.h"
#include "color.h"
#include "list.h"
#include "vector.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct polygon { // vertices that define the polygon
  list_t *points;
  vector_t *velocity;
  double angle; // angle relative to vertical
  double rotation_speed;
  vector_t center;    // center of the polygon
  rgb_color_t *color; // color of the polygon, must be freed
};

polygon_t *polygon_init(list_t *points, vector_t initial_velocity,
                        double rotation_speed, double red, double green,
                        double blue) {
  // Allocate space for the actual struct
  polygon_t *poly = malloc(sizeof(polygon_t));
  assert(poly != NULL);
  // Initialize parameters
  poly->points = points;
  poly->angle = 0;
  poly->velocity = malloc(sizeof(vector_t));
  assert(poly->velocity != NULL);
  *poly->velocity = initial_velocity;
  poly->rotation_speed = rotation_speed;
  poly->center = polygon_centroid(poly);
  poly->color = color_init(red, green, blue);

  return poly;
}

list_t *polygon_get_points(polygon_t *polygon) { return polygon->points; }

void polygon_move(polygon_t *polygon, double time_elapsed) {

  // Calculate the translational distance
  vector_t translation =
      vec_multiply(time_elapsed, *polygon_get_velocity(polygon));

  // Calculate the rotational angle
  double rotation = time_elapsed * polygon->rotation_speed;

  // Translate the polygon
  polygon_translate(polygon, translation);

  // Rotate the polygon
  polygon_rotate(polygon, rotation, VEC_ZERO);

  // Update polygon velocity
  double newx = polygon_get_velocity(polygon)->x;
  double newy = polygon_get_velocity(polygon)->y;
  vector_t newv = (vector_t){newx, newy};
  polygon_set_velocity(polygon, newv);
}

void polygon_set_velocity(polygon_t *polygon, vector_t v) {
  *polygon->velocity = v;
}

void polygon_free(polygon_t *polygon) {
  free(polygon->velocity);
  list_free(polygon->points);
  color_free(polygon->color);
  free(polygon);
}

vector_t *polygon_get_velocity(polygon_t *polygon) { return polygon->velocity; }

double polygon_area(polygon_t *polygon) {
  size_t num_points = list_size(polygon->points);
  double area = 0;
  vector_t *v;
  vector_t *v1;
  for (size_t i = 0; i < num_points; i++) {
    // Add the determinant of two conesecutive points, which is 2D cross product
    if (i == num_points - 1) {
      // Include the shoelace wrap-around from end to start
      v = (vector_t *)list_get(polygon->points, num_points - 1);
      v1 = (vector_t *)list_get(polygon->points, 0);
    } else {
      // Else apply formula for consecutive points
      v = (vector_t *)list_get(polygon->points, i);
      v1 = (vector_t *)list_get(polygon->points, i + 1);
    }
    area += vec_cross(*v, *v1);
  }
  return 0.5 * fabs(area);
}

vector_t polygon_centroid(polygon_t *polygon) {
  vector_t centroid = {0.0, 0.0};
  list_t *points = polygon->points;
  size_t num_points = list_size(points);
  vector_t *v;
  vector_t *v1;
  for (size_t i = 0; i < num_points; i++) {
    if (i == num_points - 1) {
      // Handle wrap around from last point to first point
      v = (vector_t *)list_get(polygon->points, num_points - 1);
      v1 = (vector_t *)list_get(polygon->points, 0);
    } else {
      // Else apply formula for consecutive points
      v = (vector_t *)list_get(polygon->points, i);
      v1 = (vector_t *)list_get(polygon->points, i + 1);
    }
    vector_t sum = vec_add(*v, *v1);
    double cross_product = vec_cross(*v, *v1);
    centroid = vec_add(centroid, vec_multiply(cross_product, sum));
  }
  return vec_multiply(1 / (6.0 * polygon_area(polygon)), centroid);
}

void polygon_translate(polygon_t *polygon, vector_t translation) {
  size_t num_points = list_size(polygon->points);
  vector_t point;
  for (size_t i = 0; i < num_points; i++) {
    point = *(vector_t *)list_get(polygon->points, i);
    point = vec_add(point, translation);
    ((vector_t *)list_get(polygon->points, i))->x = point.x;
    ((vector_t *)list_get(polygon->points, i))->y = point.y;
  }
  polygon->center = polygon_centroid(polygon);
}

void polygon_rotate(polygon_t *polygon, double angle, vector_t point) {
  size_t num_points = list_size(polygon->points);
  // Translate all points to the origin
  polygon_translate(polygon, vec_negate(point));
  // Rotate all points about the origin
  vector_t vertex;
  for (size_t i = 0; i < num_points; i++) {
    vertex = *((vector_t *)list_get(polygon->points, i));
    vertex = vec_rotate(vertex, angle);
    ((vector_t *)list_get(polygon->points, i))->x = vertex.x;
    ((vector_t *)list_get(polygon->points, i))->y = vertex.y;
  }
  // Translate all points back to the rotation point
  polygon_translate(polygon, point);
  polygon->angle += angle;
}

rgb_color_t *polygon_get_color(polygon_t *polygon) { return polygon->color; }

void polygon_set_color(polygon_t *polygon, rgb_color_t *color) {
  polygon->color->r = color->r;
  polygon->color->b = color->b;
  polygon->color->b = color->g;
}

vector_t polygon_get_center(polygon_t *polygon) { return polygon->center; }

void polygon_set_center(polygon_t *polygon, vector_t centroid) {
  // polygon_translate translates by an amount, not to a point, so calulate the
  // translational difference
  polygon_translate(polygon,
                    vec_subtract(centroid, polygon_get_center(polygon)));
}

void polygon_set_rotation(polygon_t *polygon, double rot) {
  // polygon_rotate rotates by an amount, not to an angle, so calculate the
  // angular difference
  polygon_rotate(polygon, rot - polygon->angle, polygon_get_center(polygon));
}

double polygon_get_rotation(polygon_t *polygon) { return polygon->angle; }
