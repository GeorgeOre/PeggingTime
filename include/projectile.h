#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "body.h"
#include "color.h"
#include "vector.h"

#include "scene.h"

#include <stdbool.h>
#include <stddef.h>

// ENUM OF WHAT KIND OF PROJECTILE
typedef enum {
  ACTIVE_PROJECTILE,
  EXPIRED_PROJECTILE,
  PROJECTILE_TYPE_COUNT
} projectile_type_t;

typedef struct projectile_info projectile_info_t;

// Need a function for constructing the projectile
projectile_info_t *
init_projectile_info(scene_t *scene, size_t scene_index,
                     projectile_type_t projectile_type, char *projectile_name,
                     double projectile_damage, double projectile_range,
                     double projectile_speed, double projectile_durability,
                     double projectile_weight,
                     double projectile_critical_chance, double projectile_cost);

void free_projectile(projectile_type_t *projectile);

double get_projectile_info_speed(projectile_info_t *info);

body_t *create_projectile(projectile_info_t *info, vector_t weapon_pos,
                          vector_t vel);

// PROJECTILE HANDLER
void projectile_handler(body_t *projectile);

bool projectile_expired(body_t *projectile);

// Accessors
projectile_type_t get_projectile_type(body_t *projectile);
char *get_projectile_name(body_t *projectile);
double get_projectile_damage(body_t *projectile);
double get_projectile_range(body_t *projectile);
double get_projectile_speed(body_t *projectile);
double get_projectile_durability(body_t *projectile);
double get_projectile_weight(body_t *projectile);
double get_projectile_critical_chance(body_t *projectile);
double get_projectile_cost(body_t *projectile);

#endif // #ifndef __PROJECTILE_H__
