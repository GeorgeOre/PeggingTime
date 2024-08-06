#ifndef __WEAPON_H__
#define __WEAPON_H__

// Struct members
#include <stdbool.h>
#include <stddef.h>

#include "body.h"
#include "color.h"
#include "scene.h"
#include "vector.h"

typedef enum {
  MELEE_WEAPON,
  RANGED_WEAPON,
  PLAYER_WEAPON,
  WEAPON_TYPE_COUNT
} weapon_type_t;

typedef struct weapon weapon_t;

// Need a function for constructing the weapon
weapon_t *init_weapon(scene_t *scene, body_t *owner, weapon_type_t weapon_type,
                      char *weapon_name, double weapon_damage,
                      double weapon_range, double weapon_attack_speed,
                      double weapon_durability, double weapon_weight,
                      double weapon_critical_chance, double weapon_reload_time,
                      double weapon_ammo_capacity, double weapon_cost,
                      double weapon_cur_ammo, double weapon_counter);

void free_weapon(weapon_t *weapon);

// Make a reset weapon option?
void reset_weapon(weapon_t *weapon);

// WEAPON HANDLER
void weapon_attack(weapon_t *weapon, list_t *asset_list, double dt,
                   vector_t player_dir);

typedef enum {
  WEAPON_DAMAGE,
  WEAPON_RANGE,
  WEAPON_DURABILITY,
  WEAPON_WEIGHT,
  WEAPON_CRITICAL_CHANCE,
  WEAPON_RELOAD_TIME,
  WEAPON_AMMO_CAPACITY,
  WEAPON_COST,
  WEAPON_STAT_TYPE_COUNT
} weapon_stat_type_t;

void weapon_upgrade_stat(body_t *being, weapon_stat_type_t stat,
                         double augmentation);

// Accessors
weapon_type_t get_weapon_type(weapon_t *weapon);
char *get_weapon_name(weapon_t *weapon);
double get_weapon_damage(weapon_t *weapon);
double get_weapon_range(weapon_t *weapon);
double get_weapon_attack_speed(weapon_t *weapon);
double get_weapon_durability(weapon_t *weapon);
double get_weapon_weight(weapon_t *weapon);
double get_weapon_critical_chance(weapon_t *weapon);
double get_weapon_reload_time(weapon_t *weapon);
double get_weapon_ammo_capacity(weapon_t *weapon);
double get_weapon_cost(weapon_t *weapon);
double get_weapon_cur_ammo(weapon_t *weapon);
double get_weapon_counter(weapon_t *weapon);
bool get_weapon_shoot(weapon_t *weapon);

void weapon_set_owner(weapon_t *weapon, body_t *owner);

void weapon_set_shoot(weapon_t *weapon, bool result);

#endif // #ifndef __WEAPON_H__
