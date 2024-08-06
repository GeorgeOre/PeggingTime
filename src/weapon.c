#include "weapon.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "asset.h"
#include "being.h"
#include "body.h"
#include "forces.h"
#include "list.h"
#include "projectile.h"
#include "scene.h"

const char *BULLET_PATH = "assets/bullet.png";

struct weapon {
  scene_t *scene;

  body_t *owner;

  weapon_type_t weapon_type;

  char *weapon_name;

  double weapon_damage;
  double weapon_range;
  double weapon_attack_speed;
  double weapon_durability;
  double weapon_weight;
  double weapon_critical_chance;
  double weapon_reload_time;
  double weapon_ammo_capacity;
  double weapon_cost;
  double weapon_cur_ammo;
  double weapon_counter;

  bool active_shoot;
};

// Need a function for constructing the weapon
weapon_t *init_weapon(scene_t *scene, body_t *owner, weapon_type_t weapon_type,
                      char *weapon_name, double weapon_damage,
                      double weapon_range, double weapon_attack_speed,
                      double weapon_durability, double weapon_weight,
                      double weapon_critical_chance, double weapon_reload_time,
                      double weapon_ammo_capacity, double weapon_cost,
                      double weapon_cur_ammo, double weapon_counter) {
  weapon_t *weapon = malloc(sizeof(weapon_t));
  weapon->scene = scene;
  weapon->owner = owner;
  weapon->weapon_type = weapon_type;
  weapon->weapon_name = weapon_name;
  weapon->weapon_damage = weapon_damage;
  weapon->weapon_range = weapon_range;
  weapon->weapon_attack_speed = weapon_attack_speed;
  weapon->weapon_durability = weapon_durability;
  weapon->weapon_weight = weapon_weight;
  weapon->weapon_critical_chance = weapon_critical_chance;
  weapon->weapon_reload_time = weapon_reload_time;
  weapon->weapon_ammo_capacity = weapon_ammo_capacity;
  weapon->weapon_cost = weapon_cost;
  weapon->weapon_cur_ammo = weapon_cur_ammo;
  weapon->weapon_counter = weapon_counter;

  weapon->active_shoot = false;

  return weapon;
}

void free_weapon(weapon_t *weapon) {
  //   free(weapon->weapon_name);
  free(weapon);
}

void reset_weapon(weapon_t *weapon) {
  weapon->weapon_type = MELEE_WEAPON;
  weapon->weapon_name = "absolute pooper";
  weapon->weapon_damage = 0.0;
  weapon->weapon_range = 0.0;
  weapon->weapon_attack_speed = 0.0;
  weapon->weapon_durability = 0.0;
  weapon->weapon_weight = 0.0;
  weapon->weapon_critical_chance = 0.0;
  weapon->weapon_reload_time = 0.0;
  weapon->weapon_ammo_capacity = 0.0;
  weapon->weapon_cost = 0.0;
  weapon->weapon_cur_ammo = 0.0;
  weapon->weapon_counter = 0.0;
}

// weapon_attack helper functions
void shoot_bullet(weapon_t *weapon, list_t *asset_list) {
  projectile_info_t *p_info;
  p_info = init_projectile_info(weapon->scene, scene_bodies(weapon->scene),
                                ACTIVE_PROJECTILE, "bullet",
                                1,   // Damage
                                300, // Range
                                500, // Speed
                                2,   // Durability (seconds)
                                1,   // Weight
                                1,   // Crit chance
                                1    // Cost
  );

  // Shoots in the direction of the target
  vector_t owner_pos = body_get_centroid(weapon->owner);
  vector_t target_pos = body_get_centroid(being_get_target(weapon->owner));
  vector_t p_vel = vec_subtract(target_pos, owner_pos);
  p_vel = vec_multiply(
      get_projectile_info_speed(p_info) / vec_get_length(p_vel), p_vel);

  body_t *bullet = create_projectile(p_info, owner_pos, p_vel);
  scene_add_body(weapon->scene, bullet);
  // Add the asset
  asset_t *bullet_asset = asset_make_image_with_body(BULLET_PATH, bullet);
  list_add(asset_list, bullet_asset);

  // Register collision with player
  create_destructive_collision(weapon->scene, scene_get_body(weapon->scene, 0),
                               bullet);
}
void player_shoot_bullet(weapon_t *weapon, list_t *asset_list,
                         vector_t player_dir) {
  projectile_info_t *p_info;
  p_info = init_projectile_info(weapon->scene, scene_bodies(weapon->scene),
                                ACTIVE_PROJECTILE, "bullet",
                                1,   // Damage
                                300, // Range
                                500, // Speed
                                2,   // Durability (seconds)
                                1,   // Weight
                                1,   // Crit chance
                                1    // Cost
  );

  // Shoots in the player direction
  vector_t p_vel = vec_multiply(get_projectile_info_speed(p_info), player_dir);

  vector_t owner_pos = body_get_centroid(weapon->owner);
  body_t *bullet = create_projectile(p_info, owner_pos, p_vel);
  scene_add_body(weapon->scene, bullet);
  // Add the asset
  asset_t *bullet_asset = asset_make_image_with_body(BULLET_PATH, bullet);
  list_add(asset_list, bullet_asset);

  // Register collision with player
  size_t NUM_ENEMIES = 2;
  for (size_t i = 1; i < 1 + NUM_ENEMIES; i++) {
    create_destructive_collision(weapon->scene,
                                 scene_get_body(weapon->scene, i), bullet);
  }
}

void weapon_attack(weapon_t *weapon, list_t *asset_list, double dt,
                   vector_t player_dir) {
  //   printf("inside weappon attack\n");
  weapon_type_t w_type = weapon->weapon_type;

  // Update weapon counter MAYBE MOVE LATER
  weapon->weapon_counter += dt;

  switch (w_type) {
  case MELEE_WEAPON:
    // Melee weapons do not do anything for now
    break;
  case RANGED_WEAPON:
    // Ranged weapons create projectiles according to their properties
    // printf("about to handle ranged weapon\n");
    // Check if the weapon has ammo
    if (weapon->weapon_cur_ammo <= 0.1) {
      // If there is no ammo, reload if the reload time has passed
      if (weapon->weapon_counter >= weapon->weapon_reload_time) {
        // There was no ammo and it is time to reload
        weapon->weapon_cur_ammo = weapon->weapon_ammo_capacity;
        weapon->weapon_counter = 0;
      } else {
        // There was no ammo and it is not time to reload
        // printf("enemy out of ammo!\n");
      }
    } else {
      // If there is ammo, check to see if you can shoot
      if (weapon->weapon_counter >= weapon->weapon_attack_speed) {
        // There was ammo and it is time to shoot
        shoot_bullet(weapon, asset_list);
        weapon->weapon_cur_ammo--;
        weapon->weapon_counter = 0;
      } else {
        // There was ammo and but it is not time to shoot
        // printf("enemy about to shoot\n");
      }
    }
    break;
  case PLAYER_WEAPON:

    if (weapon->weapon_cur_ammo <= 0.1) {
      // If there is no ammo, reload if the reload time has passed
      if (weapon->weapon_counter >= weapon->weapon_reload_time) {
        // There was no ammo and it is time to reload
        weapon->weapon_cur_ammo = weapon->weapon_ammo_capacity;
        weapon->weapon_counter = 0;
      } else {
        // There was no ammo and it is not time to reload
        // printf("out of ammo!\n");
      }
    } else {
      // If there is ammo, check to see if you can shoot
      if (weapon->weapon_counter >= weapon->weapon_attack_speed &&
          weapon->active_shoot) {
        // There was ammo and it is time to shoot
        player_shoot_bullet(weapon, asset_list, player_dir);
        weapon->weapon_cur_ammo--;
        weapon->weapon_counter = 0;
      } else {
        // There was ammo and but it is not time to shoot
        // printf("you cant shoot that fast!\n");
      }
    }
    break;

  default:
    break;
  }
}

// Weapon upgrade
void weapon_upgrade_stat(body_t *being, weapon_stat_type_t stat,
                         double augmentation) {
  weapon_t *weapon = being_get_weapon(being);

  switch (stat) {
  case WEAPON_DAMAGE:
    weapon->weapon_damage += augmentation;
    break;
  case WEAPON_RANGE:
    weapon->weapon_range += augmentation;
    break;
  case WEAPON_DURABILITY:
    weapon->weapon_durability += augmentation;
    break;
  case WEAPON_WEIGHT:
    weapon->weapon_weight += augmentation;
    break;
  case WEAPON_CRITICAL_CHANCE:
    weapon->weapon_critical_chance += augmentation;
    break;
  case WEAPON_RELOAD_TIME:
    weapon->weapon_reload_time += augmentation;
    break;
  case WEAPON_AMMO_CAPACITY:
    weapon->weapon_ammo_capacity += augmentation;
    break;
  case WEAPON_COST:
    weapon->weapon_cost += augmentation;
    break;

  default:
    break;
  }
}

// Accessors
weapon_type_t get_weapon_type(weapon_t *weapon) { return weapon->weapon_type; }
char *get_weapon_name(weapon_t *weapon) { return weapon->weapon_name; }
double get_weapon_damage(weapon_t *weapon) { return weapon->weapon_damage; }
double get_weapon_range(weapon_t *weapon) { return weapon->weapon_range; }
double get_weapon_attack_speed(weapon_t *weapon) {
  return weapon->weapon_attack_speed;
}
double get_weapon_durability(weapon_t *weapon) {
  return weapon->weapon_durability;
}
double get_weapon_weight(weapon_t *weapon) { return weapon->weapon_weight; }
double get_weapon_critical_chance(weapon_t *weapon) {
  return weapon->weapon_critical_chance;
}
double get_weapon_reload_time(weapon_t *weapon) {
  return weapon->weapon_reload_time;
}
double get_weapon_ammo_capacity(weapon_t *weapon) {
  return weapon->weapon_ammo_capacity;
}
double get_weapon_cost(weapon_t *weapon) { return weapon->weapon_cost; }

double get_weapon_cur_ammo(weapon_t *weapon) { return weapon->weapon_cur_ammo; }

double get_weapon_counter(weapon_t *weapon) { return weapon->weapon_counter; }

bool get_weapon_shoot(weapon_t *weapon) { return weapon->active_shoot; }

void weapon_set_owner(weapon_t *weapon, body_t *owner) {
  weapon->owner = owner;
}

void weapon_set_shoot(weapon_t *weapon, bool result) {
  weapon->active_shoot = result;
}
