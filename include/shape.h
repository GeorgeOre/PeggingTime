// shape.h
#ifndef __SHAPE_H__
#define __SHAPE_H__

// This file is meant to generate basic shapes in the form of vector_t lists.

// Here are the currently supported types of shapes:
// - Circle
// - Elipse
// - Square
// - Rectangle
// - Equalateral Triangle
// - 3-4-5 Triangle

// Here are some possible features to consider implementing in the future:
// - Support for custom shape creation (like a frog or a cow)
// - Support for composite shape creation (circle + square)
// - Could add rotation to the initalization

// User utils
#include "list.h"
#include "vector.h"
// #include "body.h"
#include "color.h"
#include "polygon.h"

// Math utils
#include <math.h>

// Type utils
#include <stddef.h>
// #include <SDL2/SDL2_gfxPrimitives.h>

/**
 * Generalized function for creating a circle shape.
 *
 * @param radius the radius of the circle
 * @param center the inital center of the circle
 * @param resolution the edge resolution of the circle
 *
 * @return the desired circle list_t
 */
list_t *create_circ_shape(double radius, vector_t center, size_t resolution);

/**
 * Generalized function for creating an elipse shape.
 *
 * @param outer_radius the radius of the elipse
 * @param inner_radius the radius of the elipse
 * @param center the inital center of the elipse
 * @param resolution the edge resolution of the elipse
 * @param angle the inital rotation angle of the elipse
 *
 * @return the desired elipse list_t
 */
list_t *create_elipse_shape(double outer_radius, double inner_radius,
                            vector_t center, size_t resolution, double angle);

/**
 * Generalized function for creating an square shape.
 *
 * @param width the width of the square
 * @param center the inital center of the square
 * @param angle the inital rotation angle of the square
 *
 * @return the desired square list_t
 */
list_t *create_square_shape(double width, vector_t center, double angle);

/**
 * Generalized function for creating a rectangle shape.
 *
 * @param width the width of the rectangle
 * @param height the width of the rectangle
 * @param center the inital center of the rectangle
 * @param angle the inital rotation angle of the rectangle
 *
 * @return the desired rectangle list_t
 */
list_t *create_rectangle_shape(double width, double height, vector_t center,
                               double angle);

/**
 * Generalized function for creating a equalateral triangle shape.
 *
 * @param side the side length of the equalateral triangle
 * @param center the inital center of the equalateral triangle
 * @param angle the inital rotation angle of the equalateral triangle
 *
 * @return the desired equalateral triangle list_t
 */
list_t *create_equalateral_triangle_shape(double side, vector_t center,
                                          double angle);

/**
 * Generalized function for creating a 3-4-5 triangle shape.
 *
 * @param side the '3' side length of the 3-4-5 triangle
 * @param center the inital center of the 3-4-5 triangle
 * @param angle the inital rotation angle of the 3-4-5 triangle
 *
 * @return the desired 3-4-5 triangle list_t
 */
list_t *create_345_triangle_shape(double side, vector_t center, double angle);

#endif // #ifndef __SHAPE_H__
