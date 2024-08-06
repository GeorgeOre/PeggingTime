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

// Window constants
const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};

// Obstacle constants
const int16_t V_STEP = 40;
const rgb_color_t obs_color = (rgb_color_t){0.2, 0.2, 0.3};

const vector_t BASE_OBJ_VEL = {30, 0};
const double EXTRA_VEL_MULT = 10;
const double VEL_MULT_PROB = 0.2;

const SDL_Rect SCREEN_BOUNDING_BOX = {.x = 0, .y = 0, .w = MAX.x, .h = MAX.y};
const char *FONT_PATH = "assets/fonts/Roboto-Regular.ttf";

const size_t NUM_MENU_UI_BUTTONS = 5;
const size_t NUM_MENU_UI_SCREENS = 2;
const double VOLUME_CHANGE_AMOUNT = 0.1;

// Define what page should be displayed in the user interface
typedef enum { NO_PAGE, MENU_PAGE, SETTINGS_PAGE, HELP_PAGE } ui_page_state;

// Define what screen should be displayed
typedef enum { LOAD_SCREEN, GAME_SCREEN } screen_state;
const size_t PLAYER_NUM_POINTS = 20;
const rgb_color_t player_color = (rgb_color_t){0.1, 0.9, 0.2};

// State elements

struct state {
  key_type key_to_change;
  keybinds_t user_keybinds;
  bool game_paused;
  asset_t *load_screen;
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

void init_page_and_screen(state_t *state) {
  state->ui_pages = create_ui_pages();
  state->load_screen =
      asset_make_image("assets/load_screen.png", SCREEN_BOUNDING_BOX);
}

void reset_game_state(state_t *state) {
  state->screen = LOAD_SCREEN;
  state->ui_page_state = NO_PAGE;
  state->game_paused = false;
  state->user_keybinds.MOVE_LEFT = LEFT_ARROW;
  state->user_keybinds.MOVE_RIGHT = RIGHT_ARROW;
  state->user_keybinds.MOVE_UP = UP_ARROW;
  state->user_keybinds.MOVE_DOWN = DOWN_ARROW;
  state->user_keybinds.SHOOT = SPACE_BAR;

  sdl_mixer_play_music(state->load_screen_music);
}

void user_interface_onclick(state_t *state, double x, double y) {
  fprintf(stdout, "Screen pressed");
}
void open_menu_button_handler(state_t *state) {
  state->game_paused = true;
  state->ui_page_state = MENU_PAGE;
  fprintf(stdout, "OPEN BEING PRESSED\n");
}
void close_menu_button_handler(state_t *state) {
  state->game_paused = false;
  state->ui_page_state = NO_PAGE;
}
void quit_game_button_handler(state_t *state) { reset_game_state(state); }
void play_game_button_handler(state_t *state) {
  // Change the music
  sdl_mixer_play_music(state->game_screen_music);
  // Change the screen
  state->screen = GAME_SCREEN;
}
void settings_button_handler(state_t *state) {
  state->ui_page_state = SETTINGS_PAGE;
  fprintf(stdout, "settings pressed\n");
}
void help_button_handler(state_t *state) {
  state->ui_page_state = HELP_PAGE;
  fprintf(stdout, "help pressed\n");
}
void back_button_handler(state_t *state) { state->ui_page_state = MENU_PAGE; }

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
void on_key(char key, key_event_type_t type, double held_time, state_t *state) {
  if (type == KEY_PRESSED) {
    if (key == state->user_keybinds.MOVE_UP) {
      printf("Moved up with key %c\n", state->user_keybinds.MOVE_UP);
    } else if (key == state->user_keybinds.MOVE_DOWN) {
      printf("Moved down with key %c\n", state->user_keybinds.MOVE_DOWN);
    } else if (key == state->user_keybinds.MOVE_RIGHT) {
      printf("Moved right with key %c\n", state->user_keybinds.MOVE_RIGHT);
    } else if (key == state->user_keybinds.MOVE_LEFT) {
      printf("Moved left with key %c\n", state->user_keybinds.MOVE_LEFT);
    } else if (key == state->user_keybinds.SHOOT) {
      printf("Shoot with key %c\n", state->user_keybinds.SHOOT);
    }
  }
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
    }
    printf("Key pressed for keybinder %c\n", key);
    update_key_binds_text(state->ui_buttons_text, state->user_keybinds,
                          FONT_PATH, (rgb_color_t){255, 255, 255});
  }
  sdl_on_key((key_handler_t)on_key);
  state->key_to_change = NONE;
}
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
void init_buttons_and_text(state_t *state) {
  state->ui_buttons_text =
      create_ui_buttons_text((char *)FONT_PATH, (rgb_color_t){255, 255, 255});
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

  state->ui_buttons = create_ui_buttons(state->button_assets, ui_buttons_info);
}

void init_audio(state_t *state) {
  state->load_screen_music =
      sdl_mixer_load_music("assets/sounds/shoot_sound.wav");
  state->game_screen_music =
      sdl_mixer_load_music("assets/sounds/shoot_sound.wav");
  state->shoot_sound =
      sdl_mixer_load_sound_effect("assets/sounds/shoot_sound.wav");
}
void init_game_elements(state_t *state) {}

state_t *emscripten_init() {
  // Init essentials
  state_t *state = malloc(sizeof(state_t));
  asset_cache_init();
  sdl_init(MIN, (vector_t){1000, 500});
  sdl_on_key((key_handler_t)on_key);
  srand(time(NULL));
  // Init rest of the game
  init_buttons_and_text(state);
  init_page_and_screen(state);
  init_audio(state);
  reset_game_state(state);
  return state;
}

void update_game(state_t *state, double dt) {
  //   printf("\tscroll loaded\n");
}
void render_game(state_t *state) {
  sdl_clear();
  // Render Buttons
  switch (state->ui_page_state) {
  case (NO_PAGE): {
    // render the menu open button
    asset_render(state->ui_buttons->open_menu_button);
    break;
  }
  case (MENU_PAGE): {
    // render the QUIT GAME, UI CLOSE, SETTINGS, and HELP buttons
    asset_render(state->ui_pages->menu_page);
    menu_buttons_text_render(state->ui_buttons_text);
    asset_render(state->ui_buttons->quit_game_button);
    asset_render(state->ui_buttons->settings_button);
    asset_render(state->ui_buttons->help_button);
    asset_render(state->ui_buttons->close_menu_button);
    break;
  }
  case (SETTINGS_PAGE): {
    asset_render(state->ui_pages->settings_page);
    settings_buttons_text_render(state->ui_buttons_text);
    asset_render(state->ui_buttons->back_to_menu_button);
    asset_render(state->ui_buttons->increase_music_volume_button);
    asset_render(state->ui_buttons->decrease_music_volume_button);
    asset_render(state->ui_buttons->increase_sound_effects_volume_button);
    asset_render(state->ui_buttons->decrease_sound_effects_volume_button);
    asset_render(state->ui_buttons->change_up_key_button);
    asset_render(state->ui_buttons->change_down_key_button);
    asset_render(state->ui_buttons->change_left_key_button);
    asset_render(state->ui_buttons->change_right_key_button);
    asset_render(state->ui_buttons->change_shoot_key_button);
    break;
  }
  case (HELP_PAGE): {
    asset_render(state->ui_pages->help_page);
    asset_render(state->ui_buttons->back_to_menu_button);
    break;
  }
  }
}
bool emscripten_main(state_t *state) {
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
  sdl_show();
  return false;
}

void emscripten_free(state_t *state) {
  list_free(state->button_assets);
  sdl_mixer_free_music(state->load_screen_music);
  sdl_mixer_free_music(state->game_screen_music);
  sdl_mixer_free_sound_effect(state->shoot_sound);
  ui_pages_free(state->ui_pages);
  asset_cache_destroy();
  ui_buttons_free(state->ui_buttons);
  ui_buttons_text_free(state->ui_buttons_text);
  free(state);
}
