#include "projectile.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "color.h"
#include "list.h"
#include "scene.h"
#include "shape.h"

// Window constants
const vector_t W_MIN = {0, 0};
const vector_t W_MAX = {1000, 500};

// Projectile constants
const double BULLET_RADIUS = 50;
const size_t BULLET_RESOLUTION = 10;
const double BULLET_MASS = 1;
const rgb_color_t BULLET_COLOR = (rgb_color_t){0.0, 0.0, 0.0};

struct projectile_info {
  scene_t *scene;
  size_t scene_index;

  projectile_type_t projectile_type;

  char *projectile_name;

  double projectile_damage;
  double projectile_range;
  double projectile_speed;
  double projectile_durability;
  double projectile_weight;
  double projectile_critical_chance;
  double projectile_cost;
};

// Need a function for constructing the projectile
projectile_info_t *init_projectile_info(
    scene_t *scene, size_t scene_index, projectile_type_t projectile_type,
    char *projectile_name, double projectile_damage, double projectile_range,
    double projectile_speed, double projectile_durability,
    double projectile_weight, double projectile_critical_chance,
    double projectile_cost) {
  projectile_info_t *projectile_info = malloc(sizeof(projectile_info_t));

  projectile_info->scene = scene;
  projectile_info->scene_index = scene_index;
  projectile_info->projectile_type = projectile_type;
  projectile_info->projectile_name = projectile_name;
  projectile_info->projectile_damage = projectile_damage;
  projectile_info->projectile_range = projectile_range;
  projectile_info->projectile_speed = projectile_speed;
  projectile_info->projectile_durability = projectile_durability;
  projectile_info->projectile_weight = projectile_weight;
  projectile_info->projectile_critical_chance = projectile_critical_chance;
  projectile_info->projectile_cost = projectile_cost;

  return projectile_info;
}

void free_projectile_info(projectile_info_t *projectile_info) {
  free(projectile_info);
}

double get_projectile_info_speed(projectile_info_t *info) {
  return info->projectile_speed;
}

body_t *create_projectile(projectile_info_t *info, vector_t weapon_pos,
                          vector_t vel) {
  list_t *bullet = create_square_shape(BULLET_RADIUS, weapon_pos, 0);

  body_t *p = body_init_with_info(bullet, BULLET_MASS, BULLET_COLOR, info,
                                  (free_func_t)free_projectile_info);

  body_set_centroid(p, weapon_pos);
  body_set_velocity(p, vel);

  return p;
}

// PROJECTILE HANDLER
void projectile_handler(body_t *projectile) {

  // Make sure that the projectile is deleted if it goes off screen
  if (body_get_centroid(projectile).x < 0 ||
      body_get_centroid(projectile).y < 0 ||
      body_get_centroid(projectile).x > W_MAX.x ||
      body_get_centroid(projectile).y > W_MAX.y) {

    body_remove(projectile);

    projectile_info_t *info = body_get_info(projectile);
    info->projectile_type = EXPIRED_PROJECTILE;
  }
}

bool projectile_expired(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);

  switch (info->projectile_type) {
  case ACTIVE_PROJECTILE:
    return false;
    break;

  case EXPIRED_PROJECTILE:
    return true;
    break;

  default:
    printf("UNDEFINED BEHAVIOR\n"); // Remove later
    break;
  }

  return false;
}

// Accessors
projectile_type_t get_projectile_type(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);
  return info->projectile_type;
}
char *get_projectile_name(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);
  return info->projectile_name;
}
double get_projectile_damage(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);
  return info->projectile_damage;
}
double get_projectile_range(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);
  return info->projectile_range;
}
double get_projectile_speed(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);
  return info->projectile_speed;
}
double get_projectile_durability(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);
  return info->projectile_durability;
}
double get_projectile_weight(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);
  return info->projectile_weight;
}
double get_projectile_critical_chance(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);
  return info->projectile_critical_chance;
}
double get_projectile_cost(body_t *projectile) {
  projectile_info_t *info = body_get_info(projectile);
  return info->projectile_cost;
}
