#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asset.h"
#include "asset_cache.h"
#include "collision.h"
#include "forces.h"
#include "sdl_wrapper.h"
#include "user_interface.h"

#include "being.h"
#include "body.h"
#include "projectile.h"
#include "shape.h"
#include "vector.h"
#include "weapon.h"

#define SCREEN_X 1000.0
#define SCREEN_Y 500.0
#define WORLD_X 1000.0 * 2
#define WORLD_Y 1000.0 * 2
#define MAP_X (1000.0 * 2) / 25
#define MAP_Y (1000.0 * 2) / 25

const double MAP_FACT = 30;
const size_t NUM_LEVEL = 5;

// Window constants
const vector_t MIN = {0, 0};
const vector_t MAX = {SCREEN_X, SCREEN_Y};
const double BUFF = 200;
// Change in asset as well.
const vector_t WORLD_MAX = {WORLD_X, WORLD_Y};
const vector_t WORLD_MAP = {MAP_X, MAP_Y};
const vector_t WORLD_START = {(SCREEN_X / 2), (SCREEN_Y / 2)};
const vector_t MAP_START = {SCREEN_X - (MAP_X / 2), (MAP_Y / 2)};

const double NEAR_LEFT = (WORLD_X / 2) - 20;
const double NEAR_RIGHT = SCREEN_X - (WORLD_X / 2) + 20;
const double NEAR_TOP = SCREEN_Y - (WORLD_Y / 2) + 20;
const double NEAR_BOTTOM = (WORLD_Y / 2) - 20;

const vector_t BASE_OBJ_VEL = {30, 0};
const double EXTRA_VEL_MULT = 10;
const double VEL_MULT_PROB = 0.2;

// Player constants
const double OUTER_RADIUS = 15;
const double INNER_RADIUS = 15;
const size_t PLAYER_INDEX = 1;
const size_t PLAYER_WEAPON_INDEX = 0;
const double PLAYER_MOVEMENT_VELO = 250;

const SDL_Rect SCREEN_BOUNDING_BOX = {.x = 0, .y = 0, .w = MAX.x, .h = MAX.y};
const char *FONT_PATH = "assets/fonts/Roboto-Regular.ttf";
// constants to create invaders

// User movement parameters
const double USER_VELO = 200;

const size_t OFFSET = 3;
const size_t CIRC_NPOINTS = 4;

const size_t NUM_MENU_UI_BUTTONS = 5;
const size_t NUM_MENU_UI_SCREENS = 2;
const double VOLUME_CHANGE_AMOUNT = 0.1;

rgb_color_t WHITE = {255, 255, 255};

const size_t PLAYER_NUM_POINTS = 20;
const rgb_color_t player_color = (rgb_color_t){0.1, 0.9, 0.2};

const vector_t START_POS = {500, 200};
const vector_t RESET_POS = {500, 200};

// Weapon constants
const size_t ENEMY_WEAPONS_START = 1;
const double MAX_HEALTH = 5;

// Asset Constants
const size_t BODY_ASSETS = 5;

const char *BACKGROUNDS[5] = {
    "assets/background1.png", "assets/background2.png",
    "assets/background3.png", "assets/background4.png",
    "assets/background5.png"};

const char *ASTEROID_PATH = "assets/asteroid.png";
const char *BACKGROUND_PATH1 = "assets/background2.png";
const char *BACKGROUND_PATH2 = "assets/background1.png";
const char *TRANSPARENT_PATH = "assets/transparent.png";
const char *PLAYER_DOT_PATH = "assets/red.png";

typedef struct level {
  // scene_t *scene;
  size_t enemies;
  size_t projectiles;
  scene_t *scene;
  list_t *scene_assets;
  list_t *weapons;
} level_t;

// State elements
struct state {
  size_t health;
  list_t *levels;
  // scene stuff body ax list
  scene_t *scene;
  scene_t *map_scene;
  list_t *body_assets;
  list_t *weapons;
  asset_t *map;
  asset_t *dot;
  body_t *dot_body;
  vector_t player_pos;
  body_t *background_body;

  // env movement
  vector_t player_vel;
  vector_t player_dir;
  vector_t delta_scroll;
  double *dt;
  bool left_pressed;
  bool right_pressed;
  bool up_pressed;
  bool down_pressed;
  bool player_shoot;
  size_t level;

  int16_t points;

  list_t *health_bar;
  key_type key_to_change;
  keybinds_t user_keybinds;
  bool game_paused;
  asset_t *load_screen;
  asset_t *enemies_count_text;
  ui_buttons_t *ui_buttons;
  ui_buttons_text_t *ui_buttons_text;
  list_t *button_assets;
  ui_pages_t *ui_pages;
  ui_page_state ui_page_state;
  screen_state screen;
  Mix_Music *load_screen_music;
  Mix_Music *game_screen_music;
  Mix_Chunk *shoot_sound;
  Mix_Chunk *grass_sound;
};

double rand_double(double low, double high) {
  return (high - low) * rand() / RAND_MAX + low;
}

size_t rand_size(size_t low, size_t high) {
  return (high - low) * rand() / RAND_MAX + low;
}

void init_page_and_screen(state_t *state) {
  state->ui_pages = create_ui_pages();
  state->load_screen =
      asset_make_image("assets/load_screen.png", SCREEN_BOUNDING_BOX);
}

void reset_game_state(state_t *state) {
  state->screen = LOAD_SCREEN;
  state->ui_page_state = NO_PAGE;
  state->game_paused = false;
  state->health = MAX_HEALTH;
  state->health_bar = init_health_bar(state->health);
  sdl_mixer_play_music(state->load_screen_music);
}

void user_interface_onclick(state_t *state, double x, double y) {
  fprintf(stdout, "Screen pressed");
}
/* Handlers for changing the playing/quitting game*/
void quit_game_button_handler(state_t *state) { reset_game_state(state); }
void play_game_button_handler(state_t *state) {
  // Change the music
  sdl_mixer_play_music(state->game_screen_music);
  // Change the screen
  state->screen = GAME_SCREEN;
}
/* Handlers for the menu */
void open_menu_button_handler(state_t *state) {
  state->game_paused = true;
  state->ui_page_state = MENU_PAGE;
}
void close_menu_button_handler(state_t *state) {
  state->game_paused = false;
  state->ui_page_state = NO_PAGE;
}
void settings_button_handler(state_t *state) {
  state->ui_page_state = SETTINGS_PAGE;
}
void help_button_handler(state_t *state) { state->ui_page_state = HELP_PAGE; }
void back_button_handler(state_t *state) {
  if (state->ui_page_state == SHOP_PAGE) {
    state->ui_page_state = NO_PAGE;
  } else {
    state->ui_page_state = MENU_PAGE;
  }
}

/*Handlers for adjusting in game audio*/
void increase_music_volume_button_handler(state_t *state) {
  sdl_mixer_set_music_volume(sdl_mixer_get_music_volume() +
                             VOLUME_CHANGE_AMOUNT);
}
void decrease_music_volume_button_handler(state_t *state) {
  sdl_mixer_set_music_volume(sdl_mixer_get_music_volume() -
                             VOLUME_CHANGE_AMOUNT);
}
void increase_sound_effects_volume_button_handler(state_t *state) {
  sdl_mixer_set_sound_effects_volume(sdl_mixer_get_sound_effects_volume() +
                                     VOLUME_CHANGE_AMOUNT);
}
void decrease_sound_effects_volume_button_handler(state_t *state) {
  sdl_mixer_set_sound_effects_volume(sdl_mixer_get_sound_effects_volume() -
                                     VOLUME_CHANGE_AMOUNT);
}

void init_keybinds(state_t *state) {
  state->user_keybinds.MOVE_LEFT = LEFT_ARROW;
  state->user_keybinds.MOVE_RIGHT = RIGHT_ARROW;
  state->user_keybinds.MOVE_UP = UP_ARROW;
  state->user_keybinds.MOVE_DOWN = DOWN_ARROW;
  state->user_keybinds.SHOOT = SPACE_BAR;
}
void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  vector_t new_velocity = state->player_vel;
  // Update key pressed states and velocity on key press
  if (type == KEY_PRESSED) {
    if (key == state->user_keybinds.MOVE_LEFT) {
      state->left_pressed = true;
    } else if (key == state->user_keybinds.MOVE_RIGHT) {
      state->right_pressed = true;
    } else if (key == state->user_keybinds.MOVE_UP) {
      state->up_pressed = true;
    } else if (key == state->user_keybinds.MOVE_DOWN) {
      state->down_pressed = true;
    } else if (key == state->user_keybinds.SHOOT) {
      state->player_shoot = true;
    }
  }
  // Update key pressed states and velocity on key release
  if (type == KEY_RELEASED) {
    if (key == state->user_keybinds.MOVE_LEFT) {
      state->left_pressed = false;
    } else if (key == state->user_keybinds.MOVE_RIGHT) {
      state->right_pressed = false;
    } else if (key == state->user_keybinds.MOVE_UP) {
      state->up_pressed = false;
    } else if (key == state->user_keybinds.MOVE_DOWN) {
      state->down_pressed = false;
    }
    state->player_shoot = false;
  }
  // Calculate new velocity based on pressed keys
  new_velocity.x =
      (state->right_pressed - state->left_pressed) * PLAYER_MOVEMENT_VELO;
  new_velocity.y =
      (state->up_pressed - state->down_pressed) * PLAYER_MOVEMENT_VELO;
  state->player_vel = new_velocity;
}

bool key_is_valid(char key, state_t *state) {
  bool key_is_not_keybind = (key != state->user_keybinds.MOVE_DOWN) &&
                            (key != state->user_keybinds.MOVE_UP) &&
                            (key != state->user_keybinds.MOVE_RIGHT) &&
                            (key != state->user_keybinds.MOVE_LEFT) &&
                            (key != state->user_keybinds.SHOOT);
  bool key_is_letter = key >= (int)'a' && key <= (int)'z';
  bool key_is_arrow_or_space = key == UP_ARROW || key == DOWN_ARROW ||
                               key == RIGHT_ARROW || key == LEFT_ARROW ||
                               key == SPACE_BAR;
  return key_is_not_keybind && (key_is_letter || key_is_arrow_or_space);
}
void key_bind_change_handler(char key, key_event_type_t type, double held_time,
                             state_t *state) {
  if (type == KEY_PRESSED && key_is_valid(key, state)) {
    switch (state->key_to_change) {
    case MOVE_UP: {
      state->user_keybinds.MOVE_UP = key;
      break;
    }
    case MOVE_DOWN: {
      state->user_keybinds.MOVE_DOWN = key;
      break;
    }
    case MOVE_LEFT: {
      state->user_keybinds.MOVE_LEFT = key;
      break;
    }

    case MOVE_RIGHT: {
      state->user_keybinds.MOVE_RIGHT = key;
      break;
    }
    case SHOOT: {
      state->user_keybinds.SHOOT = key;
      break;
    }
    default: {
      break;
    }
    }
    update_key_binds_text(state->ui_buttons_text, state->user_keybinds,
                          FONT_PATH, WHITE);
  }
  sdl_on_key((key_handler_t)on_key);
  state->key_to_change = NONE;
}

/*Handlers for keybind changing buttons*/
void change_up_key_button_handler(state_t *state) {
  state->key_to_change = MOVE_UP;
  sdl_on_key((key_handler_t)key_bind_change_handler);
}
void change_left_key_button_handler(state_t *state) {
  state->key_to_change = MOVE_LEFT;
  sdl_on_key((key_handler_t)key_bind_change_handler);
}
void change_right_key_button_handler(state_t *state) {
  state->key_to_change = MOVE_RIGHT;
  sdl_on_key((key_handler_t)key_bind_change_handler);
}
void change_down_key_button_handler(state_t *state) {
  state->key_to_change = MOVE_DOWN;
  sdl_on_key((key_handler_t)key_bind_change_handler);
}
void change_shoot_key_button_handler(state_t *state) {
  state->key_to_change = SHOOT;
  sdl_on_key((key_handler_t)key_bind_change_handler);
}
/*Shop buttons handlers*/
void shop_menu_button_handler(state_t *state) {
  state->ui_page_state = SHOP_PAGE;
  update_shop_info_text(state->ui_buttons_text, 0, FONT_PATH, WHITE);
}
void shop_upgrade1_button_handler(state_t *state) {
  update_shop_info_text(state->ui_buttons_text, 0, FONT_PATH, WHITE);
}
size_t randy(size_t low, size_t high) { return rand_double(low, high); }
void shop_upgrade2_button_handler(state_t *state) {
  update_shop_info_text(state->ui_buttons_text, 0, FONT_PATH, WHITE);
}
void shop_upgrade3_button_handler(state_t *state) {
  update_shop_info_text(state->ui_buttons_text, 0, FONT_PATH, WHITE);
}
// Initialize all buttons and text
void init_buttons_and_text(state_t *state) {
  state->ui_buttons_text = create_ui_buttons_text(FONT_PATH, WHITE);
  state->button_assets =
      list_init(NUM_MENU_UI_BUTTONS + 1, (free_func_t)asset_destroy);
  // Set the button handlers
  ui_buttons_info_t ui_buttons_info = get_ui_buttons_info();
  ui_buttons_info.open_menu_button_info.handler =
      (void *)open_menu_button_handler;
  ui_buttons_info.close_menu_button_info.handler =
      (void *)close_menu_button_handler;
  ui_buttons_info.settings_button_info.handler =
      (void *)settings_button_handler;
  ui_buttons_info.help_button_info.handler = (void *)help_button_handler;
  ui_buttons_info.quit_game_button_info.handler =
      (void *)quit_game_button_handler;
  ui_buttons_info.back_to_menu_button_info.handler =
      (void *)back_button_handler;
  ui_buttons_info.play_game_button_info.handler =
      (void *)play_game_button_handler;
  ui_buttons_info.increase_music_volume_button_info.handler =
      (void *)increase_music_volume_button_handler;
  ui_buttons_info.decrease_music_volume_button_info.handler =
      (void *)decrease_music_volume_button_handler;
  ui_buttons_info.increase_sound_effects_volume_button_info.handler =
      (void *)increase_sound_effects_volume_button_handler;
  ui_buttons_info.decrease_sound_effects_volume_button_info.handler =
      (void *)decrease_sound_effects_volume_button_handler;
  ui_buttons_info.change_down_key_button_info.handler =
      (void *)change_down_key_button_handler;
  ui_buttons_info.change_up_key_button_info.handler =
      (void *)change_up_key_button_handler;
  ui_buttons_info.change_left_key_button_info.handler =
      (void *)change_left_key_button_handler;
  ui_buttons_info.change_right_key_button_info.handler =
      (void *)change_right_key_button_handler;
  ui_buttons_info.change_shoot_key_button_info.handler =
      (void *)change_shoot_key_button_handler;
  ui_buttons_info.shop_button_info.handler = (void *)shop_menu_button_handler;
  ui_buttons_info.shop_upgrade1_button_info.handler =
      (void *)shop_upgrade1_button_handler;
  ui_buttons_info.shop_upgrade2_button_info.handler =
      (void *)shop_upgrade2_button_handler;
  ui_buttons_info.shop_upgrade3_button_info.handler =
      (void *)shop_upgrade3_button_handler;
  state->ui_buttons = create_ui_buttons(state->button_assets, ui_buttons_info);
  state->enemies_count_text = init_enemies_count(FONT_PATH, WHITE);
}

void edge_level(state_t *state, bool near_left, bool near_right, bool near_top,
                bool near_bottom) {

  //   printf("state level is acually %zu/%zu\n", state->level,
  //          list_size(state->levels));

  level_t *cur_level = (level_t *)list_get(state->levels, state->level);

  body_t *player_body = scene_get_body(cur_level->scene, 1);
  vector_t centroid = body_get_centroid(scene_get_body(cur_level->scene, 1));
  vector_t delta_player = VEC_ZERO;
  size_t level = randy(0, NUM_LEVEL);

  if ((centroid.x >= MAX.x) && near_right) {
    delta_player.x = -BUFF + (centroid.x - MAX.x);
    state->level = level;
    cur_level = (level_t *)list_get(state->levels, state->level);
    printf("Level %zu. %zu enemies remaining.\n", level, cur_level->enemies);
  } else if ((centroid.x <= MIN.x) && near_left) {
    delta_player.x = BUFF + (centroid.x - MIN.x);
    state->level = level;
    cur_level = (level_t *)list_get(state->levels, state->level);
    printf("Level %zu. %zu enemies remaining.\n", level, cur_level->enemies);
  } else if ((centroid.y >= MAX.y) && near_top) {
    delta_player.y = -BUFF / 2 + (centroid.y - MAX.y);
    state->level = level;
    cur_level = (level_t *)list_get(state->levels, state->level);
    printf("Level %zu. %zu enemies remaining.\n", level, cur_level->enemies);
  } else if ((centroid.y <= MIN.y) && near_bottom) {
    delta_player.y = BUFF / 2 + (centroid.y - MIN.y);
    state->level = level;
    cur_level = (level_t *)list_get(state->levels, state->level);
    printf("Level %zu. %zu enemies remaining.\n", level, cur_level->enemies);
  }
  body_set_centroid(player_body, vec_add(centroid, delta_player));
}

void reset_player(body_t *body) { body_set_centroid(body, RESET_POS); }

void reset_player_handler(body_t *body1, body_t *body2, vector_t axis,
                          void *aux, double force_const) {
  //   reset_player(body1);
  fprintf(stdout, "Collision!\n");
}

void init_audio(state_t *state) {
  state->load_screen_music =
      sdl_mixer_load_music("assets/sounds/load_screen_music.wav");
  state->game_screen_music =
      sdl_mixer_load_music("assets/sounds/game_screen_music.wav");
  state->shoot_sound =
      sdl_mixer_load_sound_effect("assets/sounds/shoot_sound.wav");
}

// void init_background(state_t *state) {
//   list_t *border =
//       create_rectangle_shape(WORLD_MAX.x, WORLD_MAX.y, WORLD_START, 0.0);

//   body_t *body = body_init(border, 1, *color_get_random());
//   state->background_body = body;
//   list_add(state->body_assets,
//            asset_make_back_image_with_body(BACKGROUND_PATH, body));
//   body_set_centroid(body, WORLD_START);
// }

void init_map(state_t *state) {
  list_t *map =
      create_rectangle_shape(WORLD_MAP.x, WORLD_MAP.y, MAP_START, 0.0);
  rgb_color_t *map_color = color_init(0, 1, 0);
  body_t *map_body = body_init(map, 1, *map_color);
  body_set_centroid(map_body, MAP_START);
  state->map = asset_make_back_image_with_body(TRANSPARENT_PATH, map_body);
  list_t *player = create_rectangle_shape(WORLD_MAP.x / 10, WORLD_MAP.y / 10,
                                          MAP_START, 0.0);
  rgb_color_t *color = color_init(1, 0, 0);
  body_t *body = body_init(player, 1, *color);
  body_set_centroid(body, MAP_START);
  state->dot = asset_make_image_with_body(TRANSPARENT_PATH, body);
  state->dot_body = body;
}

void update_map(state_t *state) {

  level_t *cur_level = (level_t *)list_get(state->levels, state->level);

  vector_t back_pos =
      vec_negate(body_get_centroid(scene_get_body(cur_level->scene, 0)));
  vector_t delta_scroll = vec_add(back_pos, state->player_pos);
  delta_scroll = vec_multiply((1 / MAP_FACT), delta_scroll);
  body_set_centroid(scene_get_body(cur_level->scene, 3),
                    vec_add(MAP_START, delta_scroll));
}

void register_all_collisions(state_t *state) {
  level_t *cur_level = (level_t *)list_get(state->levels, state->level);
  for (size_t i = PLAYER_INDEX + 1; i < scene_bodies(cur_level->scene); i++) {
    create_destructive_collision(cur_level->scene,
                                 scene_get_body(cur_level->scene, PLAYER_INDEX),
                                 scene_get_body(cur_level->scene, i));
  }
}

// level_t *level_type_init(state_t *state) {
//   level_t *level = malloc(sizeof(level_t));
//   level->enemies = 0.0;
//   level->projectiles = 0.0;
//   return level;
// }
// void level_add_scene(level_t *level) {
//   scene_t *new_scene = scene_init();
//   level->scene = new_scene;
// }

// THIS IS TO INIT AN INDIVIDUAL LEVEL
level_t *init_level_with_scene(state_t *state, const char *bg_path,
                               size_t level) {
  // Init new level
  level_t *new_level = malloc(sizeof(level_t));
  // Init non pointer params
  new_level->enemies = 0.0;
  new_level->projectiles = 0.0;

  // Init scene
  scene_t *new_scene = scene_init();
  new_level->scene = new_scene;

  // Init body asset list
  new_level->scene_assets = list_init(1, (free_func_t)asset_destroy);

  // Init weapon list
  new_level->weapons = list_init(1, (free_func_t)free_weapon);

  // Init background
  list_t *border =
      create_rectangle_shape(WORLD_MAX.x, WORLD_MAX.y, WORLD_START, 0.0);
  body_t *back_body = body_init(border, 1, *color_get_random());
  scene_add_body(new_level->scene, back_body);
  list_add(new_level->scene_assets,
           asset_make_back_image_with_body(bg_path, back_body));

  // Create the player
  body_t *player =
      init_player_being(new_level->scene, "player name", new_level->weapons,
                        new_level->scene_assets);

  // Create Map
  list_t *map =
      create_rectangle_shape(WORLD_MAP.x, WORLD_MAP.y, MAP_START, 0.0);
  rgb_color_t *map_color = color_init(0, 1, 0);
  body_t *map_body = body_init(map, 1, *map_color);
  body_set_centroid(map_body, MAP_START);
  scene_add_body(new_level->scene, map_body);
  list_add(new_level->scene_assets,
           asset_make_back_image_with_body(bg_path, map_body));

  list_t *player_dot = create_rectangle_shape(WORLD_MAP.x / 10,
                                              WORLD_MAP.y / 10, MAP_START, 0.0);
  rgb_color_t *color = color_init(1, 0, 0);
  body_t *player_dot_body = body_init(player_dot, 1, *color);
  body_set_centroid(player_dot_body, MAP_START);
  scene_add_body(new_level->scene, player_dot_body);
  list_add(new_level->scene_assets,
           asset_make_back_image_with_body(PLAYER_DOT_PATH, player_dot_body));

  for (size_t i = 0; i < 3 * (level + 1); i++) {
    // Create the Melee AI enemy
    body_t *melee_ai = init_melee_ai(
        new_level->scene, level, new_level->weapons, new_level->scene_assets);
    new_level->enemies++;

    // Create the Ranged AI
    body_t *ranged_ai = init_ranged_ai(
        new_level->scene, level, new_level->weapons, new_level->scene_assets);
    new_level->enemies++;

    //   being_set_target(player, ranged_ai);
  }

  return new_level;
}

// THIS IS TO FREE AN INDIVIDUAL LEVEL
void free_level_type(level_t *level) {
  // scene_free(level->scene);
  free(level);
}

list_t *state_levels_init(state_t *state, size_t num_levels) {
  list_t *new_level_list = list_init(num_levels, (free_func_t)free_level_type);

  for (size_t i = 0; i < num_levels; i++) {
    level_t *new_level =
        (level_t *)init_level_with_scene(state, BACKGROUNDS[i], i);
    list_add(new_level_list, new_level);
  }

  return new_level_list;
}

void init_game_elements(state_t *state) {
  state->levels = state_levels_init(
      state, NUM_LEVEL); // maybe make init or free for level struct later
  level_t *first_level = (level_t *)list_get(state->levels, 0);
  state->scene = first_level->scene;
  state->map_scene = scene_init();
  state->body_assets = list_init(BODY_ASSETS, (free_func_t)asset_destroy);
  state->weapons = list_init(1, NULL); // TODO: MAGIC NUMBER REMOVE MAYBE
  state->player_vel = VEC_ZERO;
  state->player_pos = START_POS;
  state->player_dir = (vector_t){0.0, -1.0};
  state->delta_scroll = VEC_ZERO;
  state->level = 0;
  state->player_shoot = false;
  state->left_pressed = false;
  state->right_pressed = false;
  state->up_pressed = false;
  state->down_pressed = false;
  // Create and save the asset for the background image
  //   SDL_Rect rect;
  //   rect.x = -500;
  //   rect.y = -250;
  //   rect.w = MAX.x;
  //   rect.h = MAX.y;
  //   list_add(state->body_assets, asset_make_image(BACKGROUND_PATH, rect));
  // init_background(state);
  init_map(state);

  ////////////////BEING BODY CREATION////////////////

  // level_t *cur_level = list_get(state->levels, state->level);

  //   // Create the player
  //   body_t *player = init_player_being(state->scene, "player name",
  //                                      state->weapons,  state->body_assets);

  //   state->player_vel = VEC_ZERO;
  //   state->player_pos = START_POS;
  //   init_map(state);
}
bool check_win(state_t *state) {
  bool win = true;
  for (size_t i = 0; i < list_size(state->levels); i++) {
    win = win && (((level_t *)(list_get(state->levels, i)))->enemies == 0);
    if (!win) {
      break;
    }
  }
  return win;
}

void scroll_manager(state_t *state, double dt) {

  level_t *cur_level = (level_t *)list_get(state->levels, state->level);

  vector_t player_position = state->player_pos;
  vector_t background_position =
      body_get_centroid(scene_get_body(cur_level->scene, 0));

  vector_t *delta_scroll = &(state->delta_scroll);

  *delta_scroll = vec_multiply(dt, state->player_vel);

  //   printf("SCROLL CALCULATED\n");

  bool near_left = background_position.x >= NEAR_LEFT;
  bool near_right = background_position.x <= NEAR_RIGHT;
  bool near_bottom = background_position.y >= NEAR_BOTTOM;
  bool near_top = background_position.y <= NEAR_TOP;

  if ((near_left &&
       ((state->player_vel.x <= 0) || (player_position.x < MAX.x / 2))) ||
      (near_right &&
       ((state->player_vel.x >= 0) || ((player_position.x > MAX.x / 2))))) {
    body_set_centroid(
        scene_get_body(cur_level->scene, 1),
        (vector_t){player_position.x + delta_scroll->x, player_position.y});
    player_position.x += delta_scroll->x;
    delta_scroll->x = 0.0;
  }

  if ((near_bottom &&
       ((state->player_vel.y <= 0) || (player_position.y < MAX.y / 2))) ||
      (near_top &&
       ((state->player_vel.y >= 0) || ((player_position.y > MAX.y / 2))))) {
    body_set_centroid(
        scene_get_body(cur_level->scene, 1),
        ((vector_t){player_position.x, player_position.y + delta_scroll->y}));
    delta_scroll->y = 0.0;
  }
  edge_level(state, near_left, near_right, near_top, near_bottom);
  //   printf("SCROLL MANAGER LIST\n");
}

state_t *emscripten_init() {
  // Init essentials
  state_t *state = malloc(sizeof(state_t));
  asset_cache_init();
  sdl_init(MIN, (vector_t){1000, 500});
  sdl_on_key((key_handler_t)on_key);
  srand(time(NULL));
  // Init rest of the game
  init_game_elements(state);
  register_all_collisions(state);
  init_buttons_and_text(state);
  init_page_and_screen(state);
  init_audio(state);
  init_keybinds(state);
  reset_game_state(state);
  return state;
}

void update_game(state_t *state, double dt) {
  level_t *cur_level = (level_t *)list_get(state->levels, state->level);
  //   printf("UPDATE START\n");

  // Handle AI movement
  if (cur_level->enemies != 0) {
    for (size_t i = PLAYER_INDEX + 3; i < PLAYER_INDEX + 3 + cur_level->enemies;
         i++) {
      //   printf("We are handling\n");
      body_t *being = scene_get_body(cur_level->scene, i);
      being_move(being);
    }
  }

  // printf("beings got moved\n");
  // Prep player weapon
  if (state->player_shoot) { // If the space par is being pressed
    // AND If the weapon has not been triggered to shoot already
    if (!get_weapon_shoot(
            (weapon_t *)list_get(cur_level->weapons, PLAYER_WEAPON_INDEX))) {
      // Trigger it to shoot
      weapon_set_shoot(
          (weapon_t *)list_get(cur_level->weapons, PLAYER_WEAPON_INDEX), true);
      //   printf("\t\tweapon triggered to shoot\n");
    } else {
      // If it has already been triggered, do nothing
    }
  } else { // If the spacebar is not being pressed
    // AND If the weapon has not been triggered to stop shooting already
    if (get_weapon_shoot(
            (weapon_t *)list_get(cur_level->weapons, PLAYER_WEAPON_INDEX))) {
      // Trigger it to stop shooting
      weapon_set_shoot(
          (weapon_t *)list_get(cur_level->weapons, PLAYER_WEAPON_INDEX), false);
      //   printf("\t\tweapon triggered to stop shoot\n");
    } else {
      // If it has already been untriggered, do nothing
    }
  }

  //   printf("player weapon prep handled\n");

  // Update player direction unit vector if it was changed
  if (state->up_pressed) {
    state->player_dir = (vector_t){0.0, 1.0};
    if (state->left_pressed) {
      state->player_dir = (vector_t){-sqrt(2.0) / 2.0, sqrt(2.0) / 2.0};
    } else if (state->right_pressed) {
      state->player_dir = (vector_t){sqrt(2.0) / 2.0, sqrt(2.0) / 2.0};
    }
  } else if (state->down_pressed) {
    state->player_dir = (vector_t){0.0, -1.0};
    if (state->left_pressed) {
      state->player_dir = (vector_t){-sqrt(2.0) / 2.0, -sqrt(2.0) / 2.0};
    } else if (state->right_pressed) {
      state->player_dir = (vector_t){sqrt(2.0) / 2.0, -sqrt(2.0) / 2.0};
    }
  } else {
    if (state->left_pressed) {
      state->player_dir = (vector_t){-1.0, 0.0};
    } else if (state->right_pressed) {
      state->player_dir = (vector_t){1.0, 0.0};
    }
  }

  //   printf("player dir updated (%f, %f)\n", state->player_dir.x,
  //  state->player_dir.y);
  // Handle player weapon
  size_t bodies_before = scene_bodies(cur_level->scene);
  //   printf("right b4 weapon attack\n");
  weapon_attack((weapon_t *)list_get(cur_level->weapons, PLAYER_WEAPON_INDEX),
                cur_level->scene_assets, dt, state->player_dir);
  //   printf("weapon was attacked\n");
  // Detect if a projectile was added
  size_t bodies_after = scene_bodies(cur_level->scene);
  if (bodies_before != bodies_after) {
    cur_level->projectiles++;
  }
  //   printf("player weapon handled\n");
  // Handle non-player weapons
  for (size_t i = 1; i < list_size(cur_level->weapons); i++) {
    size_t bodies_before = scene_bodies(cur_level->scene);
    weapon_attack((weapon_t *)list_get(cur_level->weapons, i),
                  cur_level->scene_assets, dt, state->player_dir);

    // Detect if a projectile was added
    size_t bodies_after = scene_bodies(cur_level->scene);
    if (bodies_before != bodies_after) {
      //   printf("\tWEAPON HANDLER: \n\t\tprojectile added so now there
      //   are %zu
      //   "
      //          "bodies\n",
      //  bodies_after);
      cur_level->projectiles++;
    }
  }
  //   printf("enemy weapon handled\n");
  // Handle projectiles
  size_t start_p_index = 4 + cur_level->enemies;
  size_t ending_p_index = 4 + cur_level->enemies + cur_level->projectiles;

  body_t *cur_projectile;
  for (ssize_t i = ending_p_index - 1; i >= start_p_index; i--) {

    // printf("\t\tPLOOOP:\n\t\t\tabout to hangle projectile at body
    // index %zu "
    //        "which should be body "
    //        "%zu/%zu\n",
    //        i, i + 1, scene_bodies(state->scene));

    // Handle the projectile
    cur_projectile = scene_get_body(cur_level->scene, i);

    projectile_handler(cur_projectile);
    // printf("\t\t\tRIGHT AFTER PROJECTILE HANGLER\n");

    // Check if they should be deleted or not
    if (projectile_expired(cur_projectile)) {
      //   scene_remove_body(state->scene, i);
      ((level_t *)list_get(state->levels, state->level))->projectiles--;

      // DONT FORGET TO REMOVE THE BODY ASSET
      list_remove(cur_level->scene_assets, i);
    }
  }

  // Handle registered body events
  scene_tick(cur_level->scene, dt);

  // Prep scrolling
  state->player_pos = body_get_centroid(scene_get_body(cur_level->scene, 1));

  scroll_manager(state, dt);
  update_map(state);
  if (check_win(state)) {
    static bool won = false;
    if (!won) {
      printf("WINNER!!!!\n");
      won = true;
    }
  }

  //   printf("UPDATE END\n");
}

void render_user_interface(state_t *state) {
  // Render Buttons
  switch (state->ui_page_state) {
  case (NO_PAGE): {
    // render the menu open button
    render_no_page(state->ui_pages, state->ui_buttons, state->ui_buttons_text);
    break;
  }
  case (MENU_PAGE): {
    render_menu_page(state->ui_pages, state->ui_buttons,
                     state->ui_buttons_text);
    break;
  }
  case (SETTINGS_PAGE): {
    render_settings_page(state->ui_pages, state->ui_buttons,
                         state->ui_buttons_text);
    break;
  }
  case (HELP_PAGE): {
    render_help_page(state->ui_pages, state->ui_buttons,
                     state->ui_buttons_text);
    break;
  }
  case (SHOP_PAGE): {
    render_shop_page(state->ui_pages, state->ui_buttons,
                     state->ui_buttons_text);
    break;
  }
  case (LEVEL_UPDATE_PAGE): {
    render_level_update_page(state->ui_pages, state->ui_buttons,
                             state->ui_buttons_text);
    break;
  }
  }
}

void render_game(state_t *state) {
  sdl_clear();

  level_t *cur_level = (level_t *)list_get(state->levels, state->level);

  size_t working_index = 0;

  // Handle and render background, scrolling if necessary
  asset_scroll_render(
      (asset_t *)list_get(cur_level->scene_assets, working_index),
      state->delta_scroll);
  working_index++;

  // Render player
  double player_angle = 0.0;
  if (state->up_pressed) {
    player_angle = 180.0;
    if (state->left_pressed) {
      player_angle -= 45.0;
    } else if (state->right_pressed) {
      player_angle += 45.0;
    }
  } else if (state->down_pressed) {
    player_angle = 0.0;
    if (state->left_pressed) {
      player_angle += 45.0;
    } else if (state->right_pressed) {
      player_angle -= 45.0;
    }
  } else {
    if (state->left_pressed) {
      player_angle = 90.0;
    } else if (state->right_pressed) {
      player_angle = -90.0;
    }
  }
  asset_rotate_render(
      (asset_t *)list_get(cur_level->scene_assets, working_index),
      player_angle);
  working_index++;

  // render map
  asset_render((asset_t *)list_get(cur_level->scene_assets, working_index));
  working_index++;

  // render map dot
  asset_render((asset_t *)list_get(cur_level->scene_assets, working_index));
  working_index++;

  // Render all enemy ai, scrolling if necessary
  for (size_t i = working_index; i < working_index + cur_level->enemies; i++) {
    asset_scroll_render((asset_t *)list_get(cur_level->scene_assets, i),
                        state->delta_scroll);
    // working_index++;
  }
  working_index = working_index + cur_level->enemies;

  //   printf("right before the p render\n");
  // Render all projectile bodies, scrolling if necessary
  if (cur_level->projectiles != 0) {
    // Make sure that projectiles exist first
    for (size_t i = working_index; i < working_index + cur_level->projectiles;
         i++) {

      asset_scroll_render((asset_t *)list_get(cur_level->scene_assets, i),
                          state->delta_scroll);
    }
  }
  render_user_interface(state);
  render_health_bar(state->health_bar, state->health);
  // asset_render(state->enemies_count_text);
}
bool emscripten_main(state_t *state) {
  //   level_t *cur_level = (level_t *)list_get(state->levels, state->level);
  //   printf(
  //       "START OF MAIN: the scene has %zu bodies, %zu assets, and %zu
  //       weapons\n", scene_bodies(cur_level->scene),
  //       list_size(cur_level->scene_assets), list_size(cur_level->weapons));
  // printf("bodies: %zu\n", scene_bodies(state->scene));

  // Fetch time
  double dt = time_since_last_tick();
  switch (state->screen) {
  case (LOAD_SCREEN): {
    asset_render(state->load_screen);
    asset_render(state->ui_buttons->play_game_button);
    break;
  }
  case (GAME_SCREEN): {
    if (!state->game_paused) {
      update_game(state, dt);
    }
    render_game(state);
    break;
  }
  }
  //   sdl_render_scene(state->map_scene, NULL);
  sdl_show();
  return false;
}

void emscripten_free(state_t *state) {
  list_free(state->body_assets);
  list_free(state->button_assets);
  list_free(state->health_bar);
  sdl_mixer_free_music(state->load_screen_music);
  sdl_mixer_free_music(state->game_screen_music);
  sdl_mixer_free_sound_effect(state->shoot_sound);
  scene_free(state->scene);
  ui_pages_free(state->ui_pages);
  asset_cache_destroy();
  ui_buttons_free(state->ui_buttons);
  ui_buttons_text_free(state->ui_buttons_text);
  free(state);
}
