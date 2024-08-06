#include "collision.h"
#include "body.h"

#include <assert.h>
#include <list.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
/**
 * Returns a list of vectors representing the edges of a shape. by
 * getting the vector between two consecutive vertices.
 *
 * @param shape the list of vectors representing the vertices of a shape
 * @return a list of vectors representing the edges of the shape
 */
static list_t *get_edges(list_t *shape) {
  list_t *edges = list_init(list_size(shape), free);
  size_t num_vertices = list_size(shape);
  for (size_t i = 0; i < num_vertices; i++) {
    vector_t *vec = malloc(sizeof(vector_t));
    assert(vec);
    *vec = vec_subtract(*(vector_t *)list_get(shape, i % num_vertices),
                        *(vector_t *)list_get(shape, (i + 1) % num_vertices));
    list_add(edges, vec);
  }
  return edges;
}

/**
 * Returns a vector containing the maximum and minimum length projections of
 * vertices in the provided shape onto the given unit axis.
 *
 * @param shape the list of vectors representing the vertices of a shape
 * @param unit_axis the unit axis to project each vertex on
 * @return a vector in the form (max, min) where `max` is the maximum projection
 * length and `min` is the minimum projection length.
 */
static vector_t get_max_min_projections(list_t *shape, vector_t unit_axis) {
  vector_t max_min = {.x = -__DBL_MAX__, .y = __DBL_MAX__};
  // For every vertex in `shape`, we should get the length of its
  // projection onto the unit axis and update the `max` and `min` variables as
  // necessary.
  double projection_length;
  size_t num_vertices = list_size(shape);
  for (size_t i = 0; i < num_vertices; i++) {
    projection_length = vec_dot(*(vector_t *)list_get(shape, i), unit_axis);
    if (projection_length > max_min.x) {
      max_min.x = projection_length;
    }
    if (projection_length < max_min.y) {
      max_min.y = projection_length;
    }
  }
  return max_min;
}

/**
 * Determines whether two convex polygons intersect.
 * The polygons are given as lists of vertices in counterclockwise order.
 * There is an edge between each pair of consecutive vertices,
 * and one between the first vertex and the last vertex.
 *
 * @param shape1 the first shape
 * @param shape2 the second shape
 * @return whether the shapes are colliding
 */
static collision_info_t compare_collision(list_t *shape1, list_t *shape2,
                                          double *min_overlap) {
  list_t *edges1 = get_edges(shape1);
  vector_t temp_axis;
  vector_t max_min_1;
  vector_t max_min_2;
  size_t num_edges = list_size(edges1);
  vector_t collision_axis = VEC_ZERO;
  double temp_overlap;
  for (size_t i = 0; i < num_edges; i++) {
    // The edge is the separating axis; the projection axis is perpendicular
    temp_axis.x = -((vector_t *)list_get(edges1, i))->y;
    temp_axis.y = ((vector_t *)list_get(edges1, i))->x;
    // Get the unit vector of the axis.
    double vec_length = vec_get_length(temp_axis);
    // Shouldn't have an axis of 0 length, but check in case.
    assert(vec_length != 0.0);
    temp_axis = vec_multiply(1 / vec_length, temp_axis);
    // Get the max and min projections for shape1 and shape2 onto the unit axis.
    max_min_1 = get_max_min_projections(shape1, temp_axis);
    max_min_2 = get_max_min_projections(shape2, temp_axis);
    // If any projections don't overlap, no collision has occured.
    if ((max_min_1.x < max_min_2.y) || (max_min_2.x < max_min_1.y)) {
      list_free(edges1);
      return (collision_info_t){.collided = false, .axis = VEC_ZERO};
    }
    // The overlap can be computed by subtracting the smallest max length
    // by the largest minimum length.
    double smallest_max =
        ((max_min_1.x) <= (max_min_2.x)) ? (max_min_1.x) : (max_min_2.x);
    double largest_min =
        ((max_min_1.y) >= (max_min_2.y)) ? (max_min_1.y) : (max_min_2.y);
    temp_overlap = (smallest_max - largest_min);
    // If there is a collision, compare the overlap to the min_overlap and
    // update the final collision axis.
    if (temp_overlap < *min_overlap) {
      collision_axis = temp_axis;
      *min_overlap = temp_overlap;
    }
  }
  // If we've reached this point, every pair of projections overlap, and thus
  // the polygons must collide.
  list_free(edges1);
  return (collision_info_t){.collided = true, .axis = collision_axis};
}

collision_info_t find_collision(body_t *body1, body_t *body2) {
  list_t *shape1 = body_get_shape(body1);
  list_t *shape2 = body_get_shape(body2);

  double c1_overlap = __DBL_MAX__;
  double c2_overlap = __DBL_MAX__;

  collision_info_t collision1 = compare_collision(shape1, shape2, &c1_overlap);
  collision_info_t collision2 = compare_collision(shape2, shape1, &c2_overlap);

  list_free(shape1);
  list_free(shape2);

  if (!collision1.collided) {
    return collision1;
  }

  if (!collision2.collided) {
    return collision2;
  }

  if (c1_overlap < c2_overlap) {
    return collision1;
  }
  return collision2;
}
