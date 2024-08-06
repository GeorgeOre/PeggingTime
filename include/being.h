#ifndef __BEING_H__
#define __BEING_H__

// Struct members
#include <stddef.h> //For size_t

#include "body.h"
#include "color.h"
#include "scene.h"
#include "vector.h"

#include "weapon.h"

/**
 * Enum representing several types of beings.
 *
 */
typedef enum {
  MELEE_BEING,
  RANGED_BEING,
  BOSS_BEING,
  PLAYER_BEING,
  BEING_TYPE_COUNT
} being_type_t;

/**
 * Struct representing a being's info.
 *
 */
typedef struct being_info being_info_t;

/**
 * Function for creating and mallocing being info.
 *
 * @param type the type of the being
 *
 * @param name the name of the being
 * @param health the health of the being
 * @param experience the experience of the being
 * @param armor the armor of the being
 * @param level the level of the being
 * @param crit_chance the critical strike chance of the being
 * @param aggression the aggression level of the being
 * @param vison_range the vison range of the being
 * @param speed the speed of the being
 * @param is_alive whether the being is alive or not
 *
 * @param weapon the weapon of the being
 * @param target the target of the being
 *
 * @return the desired being_info_t
 */
being_info_t *init_being_info(being_type_t type, char *name, double health,
                              double experience, double armor, double level,
                              double crit_chance, double aggression,
                              double vison_range, double speed, bool is_alive,
                              weapon_t *weapon, body_t *target, double points);
/**
 * Function for freeing being info.
 *
 * @param info the being info pointer
 */
void free_being_info(being_info_t *info);

/**
 * Function for fetching a being's weapon pointer.
 *
 * @param being the being whose weapon is desired
 *
 * @return the desired weapon_t pointer
 */
weapon_t *being_get_weapon(body_t *being);

/**
 * Function for fetching a being's type enum.
 *
 * @param being the being whose type is desired
 *
 * @return the desired being type
 */
being_type_t being_get_type(body_t *being);

/**
 * Function for verifying that a body is a being.
 *
 * @param body the body to test
 *
 * @return boolean representing if the body was a being
 */
bool being_verify(body_t *body);

/**
 * Function for spawning a being.
 *
 * @param info the being info for the new being body
 * @param spawn the position to spawn the new body
 *
 * @return the newly created being body
 */
body_t *being_spawn(being_info_t *info, vector_t spawn);

/**
 * Function for killing a being.
 *
 * @param being the being that needs to be killed
 */
void being_kill(body_t *being);

/**
 * Function for deleting and freeing a being.
 *
 * @param being the being that needs to be completely destroyed
 */
void being_delete(body_t *being);

/**
 * Function for resetting a being.
 *
 * @param being the being that needs to be reset
 */
void being_reset(body_t *being);

/**
 * Function for setting a being's target.
 *
 * @param being the being that is being configured
 * @param target the target being
 */
void being_set_target(body_t *being, body_t *target);

/**
 * Function for moving a being.
 *
 * @param being the being that needs to be moved
 */
void being_move(body_t *being);

/**
 * Enum representing a being's info.
 *
 */
typedef enum {
  BEING_HEALTH,
  BEING_EXP,
  BEING_ARMOR,
  BEING_LVL,
  BEING_CRIT,
  BEING_AGGRO,
  BEING_VIS,
  BEING_SPEED,
  BEING_STAT_TYPE_COUNT
} being_stat_type_t;

/**
 * Function for updating a being's stats. This modifies the being info.
 *
 * @param being the being that needs to be updated
 * @param stat the type of stat that needs to be modified
 * @param augmentation the amount to modify by
 */
void being_update_stat(body_t *being, being_stat_type_t stat,
                       double augmentation);

/**
 * Function for fetching a being's stats. This is useful for
 * modifies the being info.
 *
 * @param being the being that needs to be searched
 * @param stat the type of stat that needs to be fetched
 */
double being_get_stat(body_t *being, being_stat_type_t stat);

/**
 * Function for fetching a being's stats.
 *
 * @param being the being that needs to be updated
 *
 * @return the being's target
 */
body_t *being_get_target(body_t *being);

/**
 * Wrapper for easily facilitating player spawning.
 *
 * @param scene the scene to put the player in
 * @param name the player name
 * @param weapon_list the relevant weapon list
 * @param asset_list the relevant asset list
 *
 * @return the player being
 */
body_t *init_player_being(scene_t *scene, char *name, list_t *weapon_list,
                          list_t *asset_list);

/**
 * Wrapper for easily facilitating melee_ai spawning.
 *
 * @param scene the scene to put the melee_ai in
 * @param name the melee_ai name
 * @param weapon_list the relevant weapon list
 * @param asset_list the relevant asset list
 *
 * @return the melee_ai being
 */
body_t *init_melee_ai(scene_t *scene, double level, list_t *weapon_list,
                      list_t *asset_list);

/**
 * Wrapper for easily facilitating ranged_ai spawning.
 *
 * @param scene the scene to put the ranged_ai in
 * @param name the ranged_ai name
 * @param weapon_list the relevant weapon list
 * @param asset_list the relevant asset list
 *
 * @return the ranged_ai being
 */
body_t *init_ranged_ai(scene_t *scene, double level, list_t *weapon_list,
                       list_t *asset_list);

#endif // #ifndef __BEING_H__
