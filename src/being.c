#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "asset.h"

#include "body.h"
#include "color.h"
#include "list.h"
#include "polygon.h"
#include "shape.h"
#include "vector.h"

#include "being.h"
#include "weapon.h"

const rgb_color_t ai_color = (rgb_color_t){0.2, 1.0, 0.4};

const size_t AI_NUM_POINTS = 20;

const vector_t AI_RESET_POS = {500, 45};

const vector_t BEING_RESET_POS = {150, 150};

struct being_info {
  being_type_t type;

  char *name;

  double health;
  double experience;
  double armor;
  double level;
  double crit_chance;
  double aggression;
  double vison_range;
  double speed;
  bool is_alive;

  weapon_t *weapon;
  body_t *target;

  double points;
};

being_info_t *init_being_info(being_type_t type, char *name, double health,
                              double experience, double armor, double level,
                              double crit_chance, double aggression,
                              double vison_range, double speed, bool is_alive,
                              weapon_t *weapon, body_t *target, double points) {

  being_info_t *info = malloc(sizeof(being_info_t));

  info->type = type;
  info->name = name;
  info->health = health;
  info->experience = experience;
  info->armor = armor;
  info->level = level;
  info->crit_chance = crit_chance;
  info->aggression = aggression;
  info->vison_range = vison_range;
  info->speed = speed;
  info->is_alive = is_alive;
  info->weapon = weapon;
  info->target = target;
  info->points = points;

  return info;
}

void free_being_info(being_info_t *info) { free(info); }

weapon_t *being_get_weapon(body_t *being) {
  being_info_t *info = body_get_info(being);
  return info->weapon;
}

bool being_verify(body_t *body) {
  return (sizeof(body_get_info(body)) == sizeof(being_info_t));
}

body_t *being_spawn(being_info_t *info, vector_t spawn) {
  // For now all beings will be red squares but later this will be an enum
  // that maps to different enemy body creations
  // Maybe i can even make these things you pass in?
  double being_mass = 1;
  rgb_color_t *being_color = color_init(1, 0, 0);
  list_t *being_shape = create_square_shape(100, spawn, 0.0);

  info->is_alive = true;
  body_t *being = body_init_with_info(being_shape, being_mass, *being_color,
                                      info, (free_func_t)free_being_info);
  body_set_centroid(being, spawn);

  return being;
}

void being_kill(body_t *being) {
  being_info_t *info = body_get_info(being);
  info->is_alive = false;
}

void being_delete(body_t *being) {
  being_info_t *info = body_get_info(being);
  free_being_info(info);
  body_free(being);
}

void being_reset(body_t *being) { body_set_centroid(being, BEING_RESET_POS); }

void being_set_target(body_t *being, body_t *target) {
  being_info_t *info = body_get_info(being);
  info->target = target;
}

void being_move(body_t *being) {
  being_info_t *info = body_get_info(being);

  // Check if the being has a target or not
  if (info->target == NULL) {
    // Do nothing or just move randomly
  } else {
    // If it has a target, move according to the being type

    // Calculate movement direction
    vector_t target_pos = body_get_centroid(info->target);
    vector_t being_pos = body_get_centroid(being);
    vector_t direction = vec_subtract(target_pos, being_pos);
    // Normalize and scale direction
    double normalization =
        sqrt(pow(fabs(direction.x), 2) + pow(fabs(direction.y), 2));

    direction = vec_multiply(info->speed / normalization, direction);

    double range;
    switch (info->type) {
    case MELEE_BEING:
      body_set_velocity(being, direction);
      break;
    case RANGED_BEING:
      range = get_weapon_range(info->weapon);
      if (normalization > range) {
        // Move towards the target if out of range
        body_set_velocity(being, direction);
      } else if (normalization < range) {
        // Move away if closer than the ideal range
        direction = vec_negate(direction);
        body_set_velocity(being, direction);
      } else {
        // If the previous cases are false, don't move
        body_set_velocity(being, VEC_ZERO);
      }
      break;
    case BOSS_BEING:
      printf("This is not supposed to be implemented yet\n");
      break;

    default:
      printf("Undefined behavior\n");
      break;
    }
  }
}

void being_update_stat(body_t *being, being_stat_type_t stat,
                       double augmentation) {
  being_info_t *info = body_get_info(being);

  switch (stat) {
  case BEING_HEALTH:
    info->health += augmentation;
    break;
  case BEING_EXP:
    info->experience += augmentation;
    break;
  case BEING_ARMOR:
    info->armor += augmentation;
    break;
  case BEING_LVL:
    info->level += augmentation;
    break;
  case BEING_CRIT:
    info->crit_chance += augmentation;
    break;
  case BEING_AGGRO:
    info->aggression += augmentation;
    break;
  case BEING_VIS:
    info->vison_range += augmentation;
    break;
  case BEING_SPEED:
    info->speed += augmentation;
    break;

  default:
    break;
  }
}

double being_get_stat(body_t *being, being_stat_type_t stat) {
  being_info_t *info = body_get_info(being);

  switch (stat) {
  case BEING_HEALTH:
    return info->health;
    break;
  case BEING_EXP:
    return info->experience;
    break;
  case BEING_ARMOR:
    return info->armor;
    break;
  case BEING_LVL:
    return info->level;
    break;
  case BEING_CRIT:
    return info->crit_chance;
    break;
  case BEING_AGGRO:
    return info->aggression;
    break;
  case BEING_VIS:
    return info->vison_range;
    break;
  case BEING_SPEED:
    return info->speed;
    break;

  default:
    return 0.0;
    break;
  }
}

body_t *being_get_target(body_t *being) {
  being_info_t *info = body_get_info(being);
  return info->target;
}

being_type_t being_get_type(body_t *being) {
  being_info_t *info = body_get_info(being);
  return info->type;
}

// Player initalization constants (Base unscaled values)
const double PLAYER_WEAPON_DAMAGE = 1.0;     // Damage
const double PLAYER_WEAPON_RANGE = 200.0;    // Range
const double PLAYER_WEAPON_ATK_SPD = 1.0;    // Attack Speed (s)
const double PLAYER_WEAPON_DURABILITY = 1.0; // Durability
const double PLAYER_WEAPON_WEIGHT = 1.0;     // Weight
const double PLAYER_WEAPON_CRIT = 0.2;       // Critcal Chance
const double PLAYER_WEAPON_RELOAD_T = 5.0;   // Reload Time (s)
const double PLAYER_WEAPON_AMMO_CAP = 1.0;   // Ammo Capacity
const double PLAYER_WEAPON_COST = 1.0;       // Cost

const vector_t PLAYER_SPAWN = (vector_t){500, 200};
const double PLAYER_HP = 1.0;
const double PLAYER_EXP = 1.0;
const double PLAYER_ARMOR = 1.0;
const double PLAYER_LVL = 1.0;
const double PLAYER_CRIT = 1.0;
const double PLAYER_AGGRO = 1.0;
const double PLAYER_RANGE = 100.0;
const double PLAYER_SPEED = 80.0;
const char *PLAYER_PATH = "assets/player.png";

body_t *init_player_being(scene_t *scene, char *name, list_t *weapon_list,
                          list_t *asset_list) {
  // Create the player's weapon
  weapon_t *player_weapon = init_weapon(scene, NULL, PLAYER_WEAPON,
                                        "playerpooper", // Name
                                        1.0,            // Damage
                                        200.0,          // Range
                                        0.75,           // Attack Speed
                                        1.0,            // Durability
                                        1.0,            // Weight
                                        1.0,            // Critcal Chance
                                        1.5,            // Reload Time
                                        3.0,            // Ammo Capacity
                                        1.0,            // Cost
                                        1.0,            // Current Ammo Count
                                        0.0);           // Time Counter
  list_add(weapon_list, player_weapon);

  // Create the player
  being_info_t *player_info = init_being_info(PLAYER_BEING,
                                              name,          // Name
                                              PLAYER_HP,     // Health
                                              PLAYER_EXP,    // Experience
                                              PLAYER_ARMOR,  // Armor
                                              PLAYER_LVL,    // Level
                                              PLAYER_CRIT,   // Crit Chance
                                              PLAYER_AGGRO,  // Aggression
                                              PLAYER_RANGE,  // Vision Range
                                              PLAYER_SPEED,  // Speed
                                              true,          // Alive
                                              player_weapon, // Weapon
                                              NULL,          // Target
                                              0.0);          // Points
  body_t *player = being_spawn(player_info, PLAYER_SPAWN);
  weapon_set_owner(player_weapon, player); // DONT FORGET TO REGISTER THE WEAPON
  scene_add_body(scene, player);

  list_add(asset_list, asset_make_image_with_body(PLAYER_PATH, player));
  return player;
}

size_t rand_size2(size_t low, size_t high) {
  return (high - low) * rand() / RAND_MAX + low;
}

double rand_double2(double low, double high) {
  return (high - low) * rand() / RAND_MAX + low;
}

// Melee AI constants (Base unscaled values)
const vector_t MELEE_SPAWN = (vector_t){200, 200};
const double MELEE_HP = 1.0;
const double MELEE_EXP = 1.0;
const double MELEE_ARMOR = 1.0;
const double MELEE_CRIT = 1.0;
const double MELEE_AGGRO = 1.0;
const double MELEE_RANGE = 100.0;
const double MELEE_SPEED = 80.0;
const char *MELEE_AI_PATH = "assets/boss1.png";
const double MELEE_POINTS = 10.0;
body_t *init_melee_ai(scene_t *scene, double level, list_t *weapon_list,
                      list_t *asset_list) {
  // Extract player real quick
  body_t *player = scene_get_body(scene, 1);

  being_info_t *melee_info =
      init_being_info(MELEE_BEING,
                      "Chaser",                   // Name
                      MELEE_HP + level * 1.0,     // Health
                      MELEE_EXP + level * 1.0,    // Experience
                      MELEE_ARMOR + level * 1.0,  // Armor
                      level,                      // Level
                      MELEE_CRIT + level * 1.0,   // Crit Chance
                      MELEE_AGGRO + level * 1.0,  // Aggression
                      MELEE_RANGE + level * 1.0,  // Vision Range
                      MELEE_SPEED + level * 10.0, // Speed
                      true,                       // Alive
                      NULL,                       // Weapon
                      player,                     // Target
                      MELEE_POINTS * level);      // Points
  body_t *melee_ai =
      being_spawn(melee_info, (vector_t){rand_double2(0.0, 1000.0),
                                         rand_double2(0.0, 500.0)});
  scene_add_body(scene, melee_ai);

  // Create and save melee ai asset
  asset_t *being_asset = asset_make_image_with_body(MELEE_AI_PATH, melee_ai);
  list_add(asset_list, being_asset);

  return melee_ai;
}

// Ranged AI constants (Base unscaled values)
const double RANGED_WEAPON_DAMAGE = 1.0;     // Damage
const double RANGED_WEAPON_RANGE = 200.0;    // Range
const double RANGED_WEAPON_ATK_SPD = 1.0;    // Attack Speed (s)
const double RANGED_WEAPON_DURABILITY = 1.0; // Durability
const double RANGED_WEAPON_WEIGHT = 1.0;     // Weight
const double RANGED_WEAPON_CRIT = 0.2;       // Critcal Chance
const double RANGED_WEAPON_RELOAD_T = 5.0;   // Reload Time (s)
const double RANGED_WEAPON_AMMO_CAP = 1.0;   // Ammo Capacity
const double RANGED_WEAPON_COST = 1.0;       // Cost

const vector_t RANGED_SPAWN = (vector_t){400, 200};
const double RANGED_HP = 1.0;
const double RANGED_EXP = 1.0;
const double RANGED_ARMOR = 1.0;
const double RANGED_CRIT = 1.0;
const double RANGED_AGGRO = 1.0;
const double RANGED_RANGE = 100.0;
const double RANGED_SPEED = 80.0;
const char *RANGED_AI_PATH = "assets/boss2.png";
const double RANGED_POINTS = 5.0;
body_t *init_ranged_ai(scene_t *scene, double level, list_t *weapon_list,
                       list_t *asset_list) {
  // Extract player real quick
  body_t *player = scene_get_body(scene, 1);

  // Create the Ranged AI's weapon
  weapon_t *ranged_weapon =
      init_weapon(scene, NULL, RANGED_WEAPON,
                  "gun belonging to ai",                  // Name
                  RANGED_WEAPON_DAMAGE + level * 1.5,     // Damage
                  RANGED_WEAPON_RANGE + level * 1.0,      // Range
                  RANGED_WEAPON_ATK_SPD - (level * 0.19), // Attack Speed
                  RANGED_WEAPON_DURABILITY + level * 0.1, // Durability
                  RANGED_WEAPON_WEIGHT + level * 0.5,     // Weight
                  RANGED_WEAPON_CRIT + level * 0.2,       // Critcal Chance
                  RANGED_WEAPON_RELOAD_T - (level * 1.0), // Reload Time
                  RANGED_WEAPON_AMMO_CAP + level * 1.0,   // Ammo Capacity
                  RANGED_WEAPON_COST + level * 1.0,       // Cost
                  0.0,                                    // Current Ammo Count
                  0.0);                                   // Time Counter
  list_add(weapon_list, ranged_weapon);

  // Create the Ranged AI
  being_info_t *ranged_info =
      init_being_info(RANGED_BEING,
                      "toiletman",                 // Name
                      RANGED_HP + level * 1.5,     // Health
                      RANGED_EXP + level * 1.0,    // Experience
                      RANGED_ARMOR + level * 1.0,  // Armor
                      level,                       // Level
                      RANGED_CRIT + level * 1.0,   // Crit Chance
                      RANGED_AGGRO + level * 1.0,  // Aggression
                      RANGED_RANGE + level * 10.0, // Vision Range
                      RANGED_SPEED + level * 5.0,  // Speed
                      true,                        // Alive
                      ranged_weapon,               // Weapon
                      player,                      // Target
                      RANGED_POINTS * level);      // Points
  body_t *ranged_ai =
      being_spawn(ranged_info, (vector_t){rand_double2(0.0, 1000.0),
                                          rand_double2(0.0, 500.0)});

  weapon_set_owner(ranged_weapon,
                   ranged_ai); // DONT FORGET TO REGISTER THE WEAPON
  scene_add_body(scene, ranged_ai);
  // Create and save ranged ai asset
  asset_t *being_asset2 = asset_make_image_with_body(RANGED_AI_PATH, ranged_ai);
  list_add(asset_list, being_asset2);

  return ranged_ai;
}
