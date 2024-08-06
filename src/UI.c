#include "user_interface.h"
#include "asset.h"
#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"
#include "vector.h"
#include <assert.h>

/** @author Steven Lei*/

/**
 * NOTE: This file is heavily messy and is mostly hardcoded. A better way
 * to handle the user interface, buttons, pages, and screens rendered would
 * be to use a dictionary of some sort to store all necessary information.
 *
 * However, for the most part, like any UI, most of the properties must be hard
 * coded anyways. I ran out of time to abstract this interface, but for the most
 * part it doeos the job in creating abstractions such that the main file
 * is not cluttered with constants and button generators.
 */
const vector_t SDL_SCREEN_DIM = {1000, 500};

// Use doubles for precision in calculation

/* Page constants*/
const double PAGE_SIZE = 0.9;
const double PAGE_WIDTH = SDL_SCREEN_DIM.x * PAGE_SIZE;
const double PAGE_HEIGHT = SDL_SCREEN_DIM.y * PAGE_SIZE;
const double PAGE_X = (SDL_SCREEN_DIM.x - PAGE_WIDTH) / 2;
const double PAGE_Y = (SDL_SCREEN_DIM.y - PAGE_HEIGHT) / 2;
const SDL_Rect PAGE_BOUNDING_BOX = {
    .x = PAGE_X, .y = PAGE_Y, .w = PAGE_WIDTH, .h = PAGE_HEIGHT};
const double MIN_PAGE_DIM =
    (PAGE_HEIGHT < PAGE_WIDTH) ? PAGE_HEIGHT : PAGE_WIDTH;
const size_t NUM_MENU_BUTTONS = 4;
const double MENU_BUTTONS_Y_SPACING = 10; // Padding space between buttons
const double MENU_BUTTONS_WIDTH =
    (MIN_PAGE_DIM - (NUM_MENU_BUTTONS - 1) * MENU_BUTTONS_Y_SPACING) /
    NUM_MENU_BUTTONS; // Scale the button size proportionally
// to the screen
const double MENU_BUTTONS_HEIGHT = MENU_BUTTONS_WIDTH;
const double MENU_BUTTONS_X_POS = PAGE_X;
const double MENU_BUTTONS_Y_TOP_POS = PAGE_Y;
const double MENU_BUTTONS_Y_OFFSET =
    MENU_BUTTONS_HEIGHT + MENU_BUTTONS_Y_SPACING; // Distance between buttons
const double MENU_TEXT_WIDTH = 400;
const double MENU_TEXT_HEIGHT = 40;
const double MENU_TEXT_SPACING = 10;
const double MENU_TEXT_X =
    MENU_BUTTONS_X_POS + MENU_BUTTONS_WIDTH + MENU_TEXT_SPACING;
const double MENU_TEXT_Y_OFFSET = (MENU_BUTTONS_HEIGHT - MENU_TEXT_HEIGHT) / 2;

const double OPEN_MENU_BUTTON_X_POS = 0;
const double OPEN_MENU_BUTTON_Y_POS = 0;
const double OPEN_MENU_BUTTON_HEIGHT = 50;
const double OPEN_MENU_BUTTON_WIDTH = 50;

const double SETTINGS_PAGE_BUTTON_WIDTH = 50;
const double SETTINGS_PAGE_BUTTON_HEIGHT = 50;
const double SETTINGS_PAGE_SPACING = 10;
const double SETTINGS_PAGE_BUTTONS_X = PAGE_X + SETTINGS_PAGE_SPACING;
const double SETTINGS_PAGE_TEXT_X = SETTINGS_PAGE_BUTTONS_X +
                                    SETTINGS_PAGE_BUTTON_WIDTH +
                                    SETTINGS_PAGE_SPACING;
const double VOLUME_BUTTONS_SPACING = 5;
const double VOLUME_BUTTONS_WIDTH = 25;
const double VOLUME_BUTTONS_HEIGHT = VOLUME_BUTTONS_WIDTH;

const double INCREASE_MUSIC_VOLUME_BUTTON_X = PAGE_X;
const double INCREASE_MUSIC_VOLUME_BUTTON_Y =
    MENU_BUTTONS_Y_TOP_POS + OPEN_MENU_BUTTON_HEIGHT;
const double DECREASE_MUSIC_VOLUME_BUTTON_X = INCREASE_MUSIC_VOLUME_BUTTON_X;
const double DECREASE_MUSIC_VOLUME_BUTTON_Y = INCREASE_MUSIC_VOLUME_BUTTON_Y +
                                              VOLUME_BUTTONS_HEIGHT +
                                              VOLUME_BUTTONS_SPACING;
const double CHANGE_KEY_OFFSET_X = 400;
const double INCREASE_SOUND_EFFECTS_VOLUME_BUTTON_X =
    INCREASE_MUSIC_VOLUME_BUTTON_X;
const double INCREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y =
    DECREASE_MUSIC_VOLUME_BUTTON_Y + VOLUME_BUTTONS_HEIGHT +
    VOLUME_BUTTONS_SPACING;
const double DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_X =
    DECREASE_MUSIC_VOLUME_BUTTON_X;
const double DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y =
    INCREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y + VOLUME_BUTTONS_HEIGHT +
    VOLUME_BUTTONS_SPACING;
const double SETTINGS_PAGE_TEXT_WIDTH = 600;
const int PLAY_GAME_BUTTON_W = 150;
const int PLAY_GAME_BUTTON_H = 150;

const char UP_KEY_BUTTON_START_TEXT[] = "Up key is ";
const char DOWN_KEY_BUTTON_START_TEXT[] = "Down key is ";
const char LEFT_KEY_BUTTON_START_TEXT[] = "Left key is ";
const char RIGHT_KEY_BUTTON_START_TEXT[] = "Right key is ";
const char SHOOT_KEY_BUTTON_START_TEXT[] = "Shoot key is ";

button_info_t play_game_button_info = {
    .image_path = "assets/buttons/play_button.png",
    .image_box = (SDL_Rect){.x = (SDL_SCREEN_DIM.x - PLAY_GAME_BUTTON_W) / 2,
                            .y = 250,
                            .w = PLAY_GAME_BUTTON_W,
                            .h = PLAY_GAME_BUTTON_H},
    .handler = NULL};

button_info_t open_menu_button_info = {
    .image_path = "assets/buttons/open_ui_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = OPEN_MENU_BUTTON_X_POS,
                            .y = OPEN_MENU_BUTTON_Y_POS,
                            .w = OPEN_MENU_BUTTON_WIDTH,
                            .h = OPEN_MENU_BUTTON_HEIGHT},
    .handler = NULL};

button_info_t close_menu_button_info = {
    .image_path = "assets/buttons/close_menu_button.png",
    .image_box = (SDL_Rect){.x = MENU_BUTTONS_X_POS,
                            .y = MENU_BUTTONS_Y_TOP_POS,
                            .w = MENU_BUTTONS_WIDTH,
                            .h = MENU_BUTTONS_HEIGHT},
    .handler = NULL};

button_info_t settings_button_info = {
    .image_path = "assets/buttons/settings_button.png",
    .font_path = NULL,
    .image_box =
        (SDL_Rect){.x = MENU_BUTTONS_X_POS,
                   .y = MENU_BUTTONS_Y_TOP_POS + MENU_BUTTONS_Y_OFFSET * 1,
                   .w = MENU_BUTTONS_WIDTH,
                   .h = MENU_BUTTONS_HEIGHT},
    .handler = NULL};

button_info_t help_button_info =
    // Help
    {.image_path = "assets/buttons/help_button.png",
     .font_path = NULL,
     .image_box =
         (SDL_Rect){.x = MENU_BUTTONS_X_POS,
                    .y = MENU_BUTTONS_Y_TOP_POS + MENU_BUTTONS_Y_OFFSET * 2,
                    .w = MENU_BUTTONS_WIDTH,
                    .h = MENU_BUTTONS_HEIGHT},
     .handler = NULL};

button_info_t quit_game_button_info =
    // Quit
    {.image_path = "assets/buttons/quit_game_button.png",
     .font_path = NULL,
     .image_box =
         (SDL_Rect){.x = MENU_BUTTONS_X_POS,
                    .y = MENU_BUTTONS_Y_TOP_POS + MENU_BUTTONS_Y_OFFSET * 3,
                    .w = MENU_BUTTONS_WIDTH,
                    .h = MENU_BUTTONS_HEIGHT},
     .handler = NULL};

// Back button to exit settings/help
button_info_t back_to_menu_button_info = {
    .image_path = "assets/buttons/back_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = MENU_BUTTONS_X_POS,
                            .y = MENU_BUTTONS_Y_TOP_POS,
                            .w = OPEN_MENU_BUTTON_WIDTH,
                            .h = OPEN_MENU_BUTTON_HEIGHT},
    .handler = NULL};

button_info_t increase_music_volume_button_info = {
    .image_path = "assets/buttons/increase_volume_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = SETTINGS_PAGE_BUTTONS_X,
                            .y = INCREASE_MUSIC_VOLUME_BUTTON_Y,
                            .w = VOLUME_BUTTONS_WIDTH,
                            .h = VOLUME_BUTTONS_HEIGHT},
    .handler = NULL};
button_info_t decrease_music_volume_button_info = {
    .image_path = "assets/buttons/decrease_volume_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = SETTINGS_PAGE_BUTTONS_X,
                            .y = DECREASE_MUSIC_VOLUME_BUTTON_Y,
                            .w = VOLUME_BUTTONS_WIDTH,
                            .h = VOLUME_BUTTONS_HEIGHT},
    .handler = NULL};

button_info_t increase_sound_effects_volume_button_info = {
    .image_path = "assets/buttons/increase_volume_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = SETTINGS_PAGE_BUTTONS_X,
                            .y = INCREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y,
                            .w = VOLUME_BUTTONS_WIDTH,
                            .h = VOLUME_BUTTONS_HEIGHT},
    .handler = NULL};
button_info_t decrease_sound_effects_volume_button_info = {
    .image_path = "assets/buttons/decrease_volume_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = SETTINGS_PAGE_BUTTONS_X,
                            .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y,
                            .w = VOLUME_BUTTONS_WIDTH,
                            .h = VOLUME_BUTTONS_HEIGHT},
    .handler = NULL};

button_info_t change_up_key_button_info = {
    .image_path = "assets/buttons/up_key_button.png",
    .font_path = NULL,
    .image_box =
        (SDL_Rect){.x = SETTINGS_PAGE_BUTTONS_X,
                   .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
                        2 * SETTINGS_PAGE_BUTTON_HEIGHT + SETTINGS_PAGE_SPACING,
                   .w = SETTINGS_PAGE_BUTTON_WIDTH,
                   .h = SETTINGS_PAGE_BUTTON_HEIGHT},
    .handler = NULL};
button_info_t change_down_key_button_info = {
    .image_path = "assets/buttons/down_key_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = SETTINGS_PAGE_BUTTONS_X,
                            .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
                                 3 * (SETTINGS_PAGE_BUTTON_HEIGHT +
                                      SETTINGS_PAGE_SPACING),
                            .w = SETTINGS_PAGE_BUTTON_WIDTH,
                            .h = SETTINGS_PAGE_BUTTON_HEIGHT},
    .handler = NULL};
button_info_t change_left_key_button_info = {
    .image_path = "assets/buttons/left_key_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = SETTINGS_PAGE_BUTTONS_X + CHANGE_KEY_OFFSET_X,
                            .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
                                 2 * (SETTINGS_PAGE_BUTTON_HEIGHT +
                                      SETTINGS_PAGE_SPACING),
                            .w = SETTINGS_PAGE_BUTTON_WIDTH,
                            .h = SETTINGS_PAGE_BUTTON_HEIGHT},
    .handler = NULL};
button_info_t change_right_key_button_info = {
    .image_path = "assets/buttons/right_key_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = SETTINGS_PAGE_BUTTONS_X + CHANGE_KEY_OFFSET_X,
                            .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
                                 3 * (SETTINGS_PAGE_BUTTON_HEIGHT +
                                      SETTINGS_PAGE_SPACING),
                            .w = SETTINGS_PAGE_BUTTON_WIDTH,
                            .h = SETTINGS_PAGE_BUTTON_HEIGHT},
    .handler = NULL};
button_info_t change_shoot_key_button_info = {
    .image_path = "assets/buttons/shoot_key_button.png",
    .font_path = NULL,
    .image_box = (SDL_Rect){.x = SETTINGS_PAGE_BUTTONS_X,
                            .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
                                 4 * (SETTINGS_PAGE_BUTTON_HEIGHT +
                                      SETTINGS_PAGE_SPACING),
                            .w = SETTINGS_PAGE_BUTTON_WIDTH,
                            .h = SETTINGS_PAGE_BUTTON_HEIGHT},
    .handler = NULL};
SDL_Rect close_menu_button_text_rect = {.x = MENU_TEXT_X,
                                        .y = MENU_BUTTONS_Y_TOP_POS +
                                             MENU_TEXT_Y_OFFSET,
                                        .w = MENU_TEXT_WIDTH,
                                        .h = MENU_TEXT_HEIGHT};
SDL_Rect quit_game_button_text_rect = {.x = MENU_TEXT_X,
                                       .y = MENU_BUTTONS_Y_TOP_POS +
                                            MENU_BUTTONS_Y_OFFSET * 3 +
                                            MENU_TEXT_Y_OFFSET,
                                       .w = MENU_TEXT_WIDTH,
                                       .h = MENU_TEXT_HEIGHT};
SDL_Rect settings_button_text_rect = {.x = MENU_TEXT_X,
                                      .y = MENU_BUTTONS_Y_TOP_POS +
                                           MENU_BUTTONS_Y_OFFSET * 1 +
                                           MENU_TEXT_Y_OFFSET,
                                      .w = MENU_TEXT_WIDTH,
                                      .h = MENU_TEXT_HEIGHT};
SDL_Rect help_button_text_rect = {.x = MENU_TEXT_X,
                                  .y = MENU_BUTTONS_Y_TOP_POS +
                                       MENU_BUTTONS_Y_OFFSET * 2 +
                                       MENU_TEXT_Y_OFFSET,
                                  .w = MENU_TEXT_WIDTH,
                                  .h = MENU_TEXT_HEIGHT};

SDL_Rect music_volume_buttons_text_rect = {.x = SETTINGS_PAGE_TEXT_X,
                                           .y = INCREASE_MUSIC_VOLUME_BUTTON_Y,
                                           .w = SETTINGS_PAGE_TEXT_WIDTH,
                                           .h = VOLUME_BUTTONS_HEIGHT * 2 +
                                                VOLUME_BUTTONS_SPACING};

SDL_Rect sound_effects_volume_buttons_text_rect = {
    .x = SETTINGS_PAGE_TEXT_X,
    .y = INCREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y,
    .w = SETTINGS_PAGE_TEXT_WIDTH,
    .h = VOLUME_BUTTONS_HEIGHT * 2 + VOLUME_BUTTONS_SPACING};

SDL_Rect key_binds_info_text1_rect = {
    .x = SETTINGS_PAGE_BUTTONS_X,
    .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
         SETTINGS_PAGE_BUTTON_HEIGHT / 2 + SETTINGS_PAGE_SPACING,
    .w = SETTINGS_PAGE_TEXT_WIDTH * 1.4,
    .h = (VOLUME_BUTTONS_HEIGHT * 2 + VOLUME_BUTTONS_SPACING) * 0.6};

SDL_Rect key_binds_info_text2_rect = {
    .x = SETTINGS_PAGE_BUTTONS_X,
    .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y + SETTINGS_PAGE_BUTTON_HEIGHT +
         2 * SETTINGS_PAGE_SPACING,
    .w = SETTINGS_PAGE_TEXT_WIDTH * 1.4,
    .h = (VOLUME_BUTTONS_HEIGHT * 2 + VOLUME_BUTTONS_SPACING) * 0.6};

SDL_Rect up_key_info_text_rect = {
    .x = SETTINGS_PAGE_TEXT_X,
    .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
         2 * (SETTINGS_PAGE_BUTTON_HEIGHT + SETTINGS_PAGE_SPACING),
    .w = SETTINGS_PAGE_TEXT_WIDTH * 0.3,
    .h = (VOLUME_BUTTONS_HEIGHT * 2 + VOLUME_BUTTONS_SPACING) * 0.6};
SDL_Rect down_key_info_text_rect = {
    .x = SETTINGS_PAGE_TEXT_X,
    .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
         3 * (SETTINGS_PAGE_BUTTON_HEIGHT + SETTINGS_PAGE_SPACING),
    .w = SETTINGS_PAGE_TEXT_WIDTH * 0.3,
    .h = (VOLUME_BUTTONS_HEIGHT * 2 + VOLUME_BUTTONS_SPACING) * 0.6};
SDL_Rect left_key_info_text_rect = {
    .x = SETTINGS_PAGE_TEXT_X + CHANGE_KEY_OFFSET_X,
    .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
         2 * (SETTINGS_PAGE_BUTTON_HEIGHT + SETTINGS_PAGE_SPACING),
    .w = SETTINGS_PAGE_TEXT_WIDTH * 0.3,
    .h = (VOLUME_BUTTONS_HEIGHT * 2 + VOLUME_BUTTONS_SPACING) * 0.6};
SDL_Rect right_key_info_text_rect = {
    .x = SETTINGS_PAGE_TEXT_X + CHANGE_KEY_OFFSET_X,
    .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
         3 * (SETTINGS_PAGE_BUTTON_HEIGHT + SETTINGS_PAGE_SPACING),
    .w = SETTINGS_PAGE_TEXT_WIDTH * 0.3,
    .h = (VOLUME_BUTTONS_HEIGHT * 2 + VOLUME_BUTTONS_SPACING) * 0.6};
SDL_Rect shoot_key_info_text_rect = {
    .x = SETTINGS_PAGE_TEXT_X,
    .y = DECREASE_SOUND_EFFECTS_VOLUME_BUTTON_Y +
         4 * (SETTINGS_PAGE_BUTTON_HEIGHT + SETTINGS_PAGE_SPACING),
    .w = SETTINGS_PAGE_TEXT_WIDTH * 0.6,
    .h = (VOLUME_BUTTONS_HEIGHT * 2 + VOLUME_BUTTONS_SPACING) * 0.6};

const double SHOP_BUTTON_WIDTH = OPEN_MENU_BUTTON_WIDTH;
const double SHOP_BUTTON_HEIGHT = OPEN_MENU_BUTTON_HEIGHT;
const double SHOP_BUTTON_X = OPEN_MENU_BUTTON_X_POS + 10 + SHOP_BUTTON_WIDTH;
;
const double SHOP_BUTTON_Y = 0;
const double SHOP_MENU_SPACING = 10;
const double SHOP_MENU_BUTTONS_Y =
    MENU_BUTTONS_Y_TOP_POS + OPEN_MENU_BUTTON_HEIGHT + SHOP_MENU_SPACING;
const double SHOP_MENU_BUTTONS_HEIGHT =
    (PAGE_HEIGHT - SHOP_MENU_BUTTONS_Y - 2 * SHOP_MENU_SPACING) / 3;
const double SHOP_MENU_BUTTONS_WIDTH = SHOP_MENU_BUTTONS_HEIGHT;
button_info_t shop_button_info = {
    .font_path = NULL,
    .image_path = "assets/buttons/shop_button.png",
    .image_box = (SDL_Rect){.x = SHOP_BUTTON_X,
                            .y = SHOP_BUTTON_Y,
                            .w = SHOP_BUTTON_WIDTH,
                            .h = SHOP_BUTTON_HEIGHT},
    .handler = NULL};

button_info_t shop_upgrade1_button_info = {
    .font_path = NULL,
    .image_path = "assets/buttons/movement_speed_button.png",
    .image_box = (SDL_Rect){.x = PAGE_X,
                            .y = SHOP_MENU_BUTTONS_Y,
                            .w = SHOP_MENU_BUTTONS_WIDTH,
                            .h = SHOP_MENU_BUTTONS_HEIGHT},
    .handler = NULL};
button_info_t shop_upgrade2_button_info = {
    .font_path = NULL,
    .image_path = "assets/buttons/attack_speed_button.png",
    .image_box = (SDL_Rect){.x = PAGE_X,
                            .y = SHOP_MENU_BUTTONS_Y + SHOP_MENU_SPACING +
                                 SHOP_MENU_BUTTONS_HEIGHT,
                            .w = SHOP_MENU_BUTTONS_WIDTH,
                            .h = SHOP_MENU_BUTTONS_HEIGHT},
    .handler = NULL};

button_info_t shop_upgrade3_button_info = {
    .font_path = NULL,
    .image_path = "assets/buttons/attack_damage_button.png",
    .image_box =
        (SDL_Rect){.x = PAGE_X,
                   .y = SHOP_MENU_BUTTONS_Y +
                        2 * (SHOP_MENU_SPACING + SHOP_MENU_BUTTONS_HEIGHT),
                   .w = SHOP_MENU_BUTTONS_WIDTH,
                   .h = SHOP_MENU_BUTTONS_HEIGHT},
    .handler = NULL};

SDL_Rect shop_info_text_rect = {.x = PAGE_X + OPEN_MENU_BUTTON_WIDTH +
                                     SHOP_MENU_SPACING,
                                .y = PAGE_Y,
                                .w = MENU_TEXT_WIDTH,
                                .h = MENU_TEXT_HEIGHT};

SDL_Rect shop_upgrade1_button_text_rect = {
    .x = PAGE_X + SHOP_MENU_BUTTONS_WIDTH + SHOP_MENU_SPACING,
    .y = SHOP_MENU_BUTTONS_Y,
    .w = MENU_TEXT_WIDTH,
    .h = MENU_TEXT_HEIGHT};

SDL_Rect shop_upgrade2_button_text_rect = {
    .x = PAGE_X + SHOP_MENU_BUTTONS_WIDTH + SHOP_MENU_SPACING,
    .y = SHOP_MENU_BUTTONS_Y + (SHOP_MENU_SPACING + SHOP_MENU_BUTTONS_HEIGHT),
    .w = MENU_TEXT_WIDTH,
    .h = MENU_TEXT_HEIGHT};

SDL_Rect shop_upgrade3_button_text_rect = {
    .x = PAGE_X + SHOP_MENU_BUTTONS_WIDTH + SHOP_MENU_SPACING,
    .y = SHOP_MENU_BUTTONS_Y +
         2 * (SHOP_MENU_SPACING + SHOP_MENU_BUTTONS_HEIGHT),
    .w = MENU_TEXT_WIDTH,
    .h = MENU_TEXT_HEIGHT};

SDL_Rect current_level_text_rect = {.x = (PAGE_WIDTH - MENU_TEXT_WIDTH) / 2,
                                    .y = (PAGE_WIDTH - MENU_TEXT_WIDTH) / 2,
                                    .w = MENU_TEXT_WIDTH,
                                    .h = MENU_TEXT_HEIGHT};
const char *SHOP_INFO_BASE_TEXT = "Buy upgrades using your current gold: ";
ui_buttons_text_t *create_ui_buttons_text(const char *font_filepath,
                                          rgb_color_t color) {
  ui_buttons_text_t *buttons_text = malloc(sizeof(ui_buttons_text_t));
  buttons_text->close_menu_button_text =
      asset_make_text(font_filepath, close_menu_button_text_rect,
                      "Close menu and resume game", color);

  buttons_text->quit_game_button_text =
      asset_make_text(font_filepath, quit_game_button_text_rect,
                      "Quit game and delete progresss", color);

  buttons_text->settings_button_text =
      asset_make_text(font_filepath, settings_button_text_rect,
                      "Change keybinds or volume", color);

  buttons_text->help_button_text =
      asset_make_text(font_filepath, help_button_text_rect,
                      "Display game information and help", color);
  buttons_text->music_volume_buttons_text =
      asset_make_text(font_filepath, music_volume_buttons_text_rect,
                      "Increase/decrease music volume", color);
  buttons_text->sound_effects_volume_buttons_text =
      asset_make_text(font_filepath, sound_effects_volume_buttons_text_rect,
                      "Increase/decrease sound effects volume", color);
  buttons_text->key_binds_info_text1 =
      asset_make_text(font_filepath, key_binds_info_text1_rect,
                      "To change keybinds, press the respective button and "
                      "then press the desired key on the keyboard.",
                      color);
  buttons_text->key_binds_info_text2 =
      asset_make_text(font_filepath, key_binds_info_text2_rect,
                      "Only letters, arrows, space, & unused keys are valid. "
                      "If an invalid key is "
                      "pressed, repress the button to set a valid key.",
                      color);
  buttons_text->up_key_info_text = asset_make_text(
      font_filepath, up_key_info_text_rect, "Up key is ^", color);
  buttons_text->down_key_info_text = asset_make_text(
      font_filepath, down_key_info_text_rect, "Down key is v", color);
  buttons_text->right_key_info_text = asset_make_text(
      font_filepath, right_key_info_text_rect, "Right key is >", color);
  buttons_text->left_key_info_text = asset_make_text(
      font_filepath, left_key_info_text_rect, "Left key is <", color);
  buttons_text->shoot_key_info_text = asset_make_text(
      font_filepath, shoot_key_info_text_rect, "Shoot key is SPACEBAR", color);
  buttons_text->shop_info_text = asset_make_text(
      font_filepath, shop_info_text_rect, SHOP_INFO_BASE_TEXT, color);
  buttons_text->shop_upgrade1_button_text =
      asset_make_text(font_filepath, shop_upgrade1_button_text_rect,
                      "Increases movement speed. Cost: 10", color);
  buttons_text->shop_upgrade2_button_text =
      asset_make_text(font_filepath, shop_upgrade2_button_text_rect,
                      "Increases attack speed. Cost: 10", color);
  buttons_text->shop_upgrade3_button_text =
      asset_make_text(font_filepath, shop_upgrade3_button_text_rect,
                      "Increases attack damage. Cost: 10", color);
  buttons_text->current_level_text = asset_make_text(
      font_filepath, current_level_text_rect, "Current level is 1", color);
  return buttons_text;
}

// Converts a key value to a string.
char *key_to_string(char key) {
  char *key_as_string = malloc(sizeof(char) * 2);
  assert(key_as_string);
  switch (key) {
  case UP_ARROW:
    key_as_string[0] = '^';
    break;
  case DOWN_ARROW:
    key_as_string[0] = 'v';
    break;
  case LEFT_ARROW:
    key_as_string[0] = '<';
    break;
  case RIGHT_ARROW:
    key_as_string[0] = '>';
    break;
  case SPACE_BAR:
    key_as_string = realloc(key_as_string, strlen("SPACEBAR") + 1);
    assert(key_as_string);
    strcpy(key_as_string, "SPACEBAR");
    return key_as_string;
  default:
    key_as_string[0] = key;
  }
  key_as_string[1] = '\0';
  return key_as_string;
}
const size_t GOLD_MAX_VALUE = 999;
void update_shop_info_text(ui_buttons_text_t *buttons_text, size_t gold,
                           const char *font_filepath, rgb_color_t color) {
  asset_destroy(buttons_text->shop_info_text);
  char gold_as_string[GOLD_MAX_VALUE];
  sprintf(gold_as_string, "%zu", gold);
  char *shop_info_text =
      malloc(strlen(gold_as_string) + strlen(SHOP_INFO_BASE_TEXT) + 1);
  assert(shop_info_text);
  strcpy(shop_info_text, SHOP_INFO_BASE_TEXT);
  strcat(shop_info_text, gold_as_string);
  buttons_text->shop_info_text = asset_make_text(
      font_filepath, shop_info_text_rect, shop_info_text, color);
}

void update_key_binds_text(ui_buttons_text_t *buttons_text, keybinds_t keybinds,
                           const char *font_filepath, rgb_color_t color) {
  asset_destroy(buttons_text->up_key_info_text);
  asset_destroy(buttons_text->down_key_info_text);
  asset_destroy(buttons_text->left_key_info_text);
  asset_destroy(buttons_text->right_key_info_text);
  asset_destroy(buttons_text->shoot_key_info_text);

  char *up_key_as_string = key_to_string(keybinds.MOVE_UP);
  char *down_key_as_string = key_to_string(keybinds.MOVE_DOWN);
  char *left_key_as_string = key_to_string(keybinds.MOVE_LEFT);
  char *right_key_as_string = key_to_string(keybinds.MOVE_RIGHT);
  char *shoot_key_as_string = key_to_string(keybinds.SHOOT);

  char *up_key_button_text =
      malloc(strlen(UP_KEY_BUTTON_START_TEXT) + strlen(up_key_as_string) + 1);
  assert(up_key_button_text);
  strcpy(up_key_button_text, UP_KEY_BUTTON_START_TEXT);
  strcat(up_key_button_text, up_key_as_string);

  char *down_key_button_text = malloc(strlen(DOWN_KEY_BUTTON_START_TEXT) +
                                      strlen(down_key_as_string) + 1);
  assert(down_key_button_text);
  strcpy(down_key_button_text, DOWN_KEY_BUTTON_START_TEXT);
  strcat(down_key_button_text, down_key_as_string);

  char *left_key_button_text = malloc(strlen(LEFT_KEY_BUTTON_START_TEXT) +
                                      strlen(left_key_as_string) + 1);
  assert(left_key_button_text);
  strcpy(left_key_button_text, LEFT_KEY_BUTTON_START_TEXT);
  strcat(left_key_button_text, left_key_as_string);

  char *right_key_button_text = malloc(strlen(RIGHT_KEY_BUTTON_START_TEXT) +
                                       strlen(right_key_as_string) + 1);
  assert(right_key_button_text);
  strcpy(right_key_button_text, RIGHT_KEY_BUTTON_START_TEXT);
  strcat(right_key_button_text, right_key_as_string);

  char *shoot_key_button_text = malloc(strlen(SHOOT_KEY_BUTTON_START_TEXT) +
                                       strlen(shoot_key_as_string) + 1);
  assert(shoot_key_button_text);
  strcpy(shoot_key_button_text, SHOOT_KEY_BUTTON_START_TEXT);
  strcat(shoot_key_button_text, shoot_key_as_string);

  buttons_text->up_key_info_text = asset_make_text(
      font_filepath, up_key_info_text_rect, up_key_button_text, color);
  buttons_text->down_key_info_text = asset_make_text(
      font_filepath, down_key_info_text_rect, down_key_button_text, color);
  buttons_text->right_key_info_text = asset_make_text(
      font_filepath, right_key_info_text_rect, right_key_button_text, color);
  buttons_text->left_key_info_text = asset_make_text(
      font_filepath, left_key_info_text_rect, left_key_button_text, color);
  buttons_text->shoot_key_info_text = asset_make_text(
      font_filepath, shoot_key_info_text_rect, shoot_key_button_text, color);
}
void menu_buttons_text_render(ui_buttons_text_t *buttons_text) {
  asset_render(buttons_text->close_menu_button_text);
  asset_render(buttons_text->quit_game_button_text);
  asset_render(buttons_text->settings_button_text);
  asset_render(buttons_text->help_button_text);
}
void settings_buttons_text_render(ui_buttons_text_t *buttons_text) {
  asset_render(buttons_text->music_volume_buttons_text);
  asset_render(buttons_text->sound_effects_volume_buttons_text);
  asset_render(buttons_text->key_binds_info_text1);
  asset_render(buttons_text->key_binds_info_text2);
  asset_render(buttons_text->up_key_info_text);
  asset_render(buttons_text->down_key_info_text);
  asset_render(buttons_text->left_key_info_text);
  asset_render(buttons_text->right_key_info_text);
  asset_render(buttons_text->shoot_key_info_text);
}
void ui_buttons_text_free(ui_buttons_text_t *buttons_text) {
  asset_destroy(buttons_text->close_menu_button_text);
  asset_destroy(buttons_text->settings_button_text);
  asset_destroy(buttons_text->help_button_text);
  asset_destroy(buttons_text->quit_game_button_text);
  asset_destroy(buttons_text->music_volume_buttons_text);
  asset_destroy(buttons_text->sound_effects_volume_buttons_text);
  asset_destroy(buttons_text->key_binds_info_text1);
  asset_destroy(buttons_text->key_binds_info_text2);
  asset_destroy(buttons_text->up_key_info_text);
  asset_destroy(buttons_text->down_key_info_text);
  asset_destroy(buttons_text->left_key_info_text);
  asset_destroy(buttons_text->right_key_info_text);
  asset_destroy(buttons_text->shoot_key_info_text);
  asset_destroy(buttons_text->shop_info_text);
  asset_destroy(buttons_text->shop_upgrade1_button_text);
  asset_destroy(buttons_text->shop_upgrade2_button_text);
  asset_destroy(buttons_text->shop_upgrade3_button_text);
  free(buttons_text);
}

ui_buttons_info_t get_ui_buttons_info() {
  ui_buttons_info_t info = (ui_buttons_info_t){
      .open_menu_button_info = open_menu_button_info,
      .close_menu_button_info = close_menu_button_info,
      .settings_button_info = settings_button_info,
      .help_button_info = help_button_info,
      .back_to_menu_button_info = back_to_menu_button_info,
      .quit_game_button_info = quit_game_button_info,
      .play_game_button_info = play_game_button_info,
      .increase_music_volume_button_info = increase_music_volume_button_info,
      .decrease_music_volume_button_info = decrease_music_volume_button_info,
      .increase_sound_effects_volume_button_info =
          increase_sound_effects_volume_button_info,
      .decrease_sound_effects_volume_button_info =
          decrease_sound_effects_volume_button_info,
      .change_down_key_button_info = change_down_key_button_info,
      .change_up_key_button_info = change_up_key_button_info,
      .change_left_key_button_info = change_left_key_button_info,
      .change_right_key_button_info = change_right_key_button_info,
      .change_shoot_key_button_info = change_shoot_key_button_info,
      .shop_button_info = shop_button_info,
      .shop_upgrade1_button_info = shop_upgrade1_button_info,
      .shop_upgrade2_button_info = shop_upgrade2_button_info,
      .shop_upgrade3_button_info = shop_upgrade3_button_info};
  return info;
}

asset_t *create_button_from_info(list_t *button_assets, button_info_t info) {
  // Ensure that there is a provided handler.
  assert(info.handler != NULL);
  // Create the button's image assets
  asset_t *image = NULL;
  if (info.image_path != NULL) {
    image = asset_make_image(info.image_path, info.image_box);
    list_add(button_assets, image);
  }
  // Create the button's text assets
  asset_t *text = NULL;
  if (info.font_path != NULL) {
    text = asset_make_text(info.font_path, info.text_box, info.text,
                           info.text_color);
    list_add(button_assets, text);
  }
  // Create the button
  asset_t *button =
      asset_make_button(info.image_box, image, text, info.handler);
  // Register the button
  asset_cache_register_button(button);
  return (asset_t *)button;
}

ui_buttons_t *create_ui_buttons(list_t *button_assets,
                                ui_buttons_info_t ui_buttons_info) {
  ui_buttons_t *ui_buttons = malloc(sizeof(ui_buttons_t));
  ui_buttons->open_menu_button = create_button_from_info(
      button_assets, ui_buttons_info.open_menu_button_info);
  ui_buttons->close_menu_button = create_button_from_info(
      button_assets, ui_buttons_info.close_menu_button_info);
  ui_buttons->settings_button = create_button_from_info(
      button_assets, ui_buttons_info.settings_button_info);
  ui_buttons->help_button =
      create_button_from_info(button_assets, ui_buttons_info.help_button_info);
  ui_buttons->back_to_menu_button = create_button_from_info(
      button_assets, ui_buttons_info.back_to_menu_button_info);
  ui_buttons->quit_game_button = create_button_from_info(
      button_assets, ui_buttons_info.quit_game_button_info);
  ui_buttons->play_game_button = create_button_from_info(
      button_assets, ui_buttons_info.play_game_button_info);
  ui_buttons->decrease_music_volume_button = create_button_from_info(
      button_assets, ui_buttons_info.decrease_music_volume_button_info);
  ui_buttons->increase_music_volume_button = create_button_from_info(
      button_assets, ui_buttons_info.increase_music_volume_button_info);
  ui_buttons->increase_sound_effects_volume_button = create_button_from_info(
      button_assets, ui_buttons_info.increase_sound_effects_volume_button_info);
  ui_buttons->decrease_sound_effects_volume_button = create_button_from_info(
      button_assets, ui_buttons_info.decrease_sound_effects_volume_button_info);
  ui_buttons->change_up_key_button = create_button_from_info(
      button_assets, ui_buttons_info.change_up_key_button_info);
  ui_buttons->change_down_key_button = create_button_from_info(
      button_assets, ui_buttons_info.change_down_key_button_info);
  ui_buttons->change_left_key_button = create_button_from_info(
      button_assets, ui_buttons_info.change_left_key_button_info);
  ui_buttons->change_right_key_button = create_button_from_info(
      button_assets, ui_buttons_info.change_right_key_button_info);
  ui_buttons->change_shoot_key_button = create_button_from_info(
      button_assets, ui_buttons_info.change_shoot_key_button_info);
  ui_buttons->change_shoot_key_button = create_button_from_info(
      button_assets, ui_buttons_info.change_shoot_key_button_info);
  // Shop
  ui_buttons->shop_button =
      create_button_from_info(button_assets, ui_buttons_info.shop_button_info);
  ui_buttons->shop_upgrade1_button = create_button_from_info(
      button_assets, ui_buttons_info.shop_upgrade1_button_info);
  ui_buttons->shop_upgrade2_button = create_button_from_info(
      button_assets, ui_buttons_info.shop_upgrade2_button_info);
  ui_buttons->shop_upgrade3_button = create_button_from_info(
      button_assets, ui_buttons_info.shop_upgrade3_button_info);
  return ui_buttons;
}

void ui_buttons_free(ui_buttons_t *ui_buttons) { free(ui_buttons); }

ui_pages_t *create_ui_pages() {
  ui_pages_t *pages = malloc(sizeof(ui_pages_t));
  pages->menu_page =
      asset_make_image("assets/pages/menu_page.png", PAGE_BOUNDING_BOX);
  pages->settings_page =
      asset_make_image("assets/pages/menu_page.png", PAGE_BOUNDING_BOX);
  pages->help_page =
      asset_make_image("assets/pages/help_page.png", PAGE_BOUNDING_BOX);
  pages->shop_page =
      asset_make_image("assets/pages/menu_page.png", PAGE_BOUNDING_BOX);
  pages->level_update_page =
      asset_make_image("assets/pages/menu_page.png", PAGE_BOUNDING_BOX);
  return pages;
}
void ui_pages_free(ui_pages_t *pages) {
  asset_destroy(pages->menu_page);
  asset_destroy(pages->settings_page);
  asset_destroy(pages->help_page);
  asset_destroy(pages->shop_page);
  asset_destroy(pages->level_update_page);
  free(pages);
}

void shop_buttons_text_render(ui_buttons_text_t *buttons_text) {
  asset_render(buttons_text->shop_info_text);
  asset_render(buttons_text->shop_upgrade1_button_text);
  asset_render(buttons_text->shop_upgrade2_button_text);
  asset_render(buttons_text->shop_upgrade3_button_text);
}
const double HEALTH_ICON_HEIGHT = OPEN_MENU_BUTTON_WIDTH;
const double HEALTH_ICON_WIDTH = OPEN_MENU_BUTTON_HEIGHT;
const double HEALTH_BARS_X = SDL_SCREEN_DIM.x - HEALTH_ICON_WIDTH;
const double HEALTH_BARS_Y = 0;
const double HEALTH_BARS_SPACING = 10;
const char *HEALTH_ICON_PATH = "assets/icons/heart_icon.png";

list_t *init_health_bar(size_t max_health) {
  list_t *health_bar = list_init(max_health, (free_func_t)asset_destroy);
  for (size_t i = 0; i < max_health; i++) {
    double health_bar_x_location =
        HEALTH_BARS_X - i * (HEALTH_BARS_SPACING + HEALTH_ICON_WIDTH);
    SDL_Rect health_icon_rect = {.x = health_bar_x_location,
                                 .y = HEALTH_BARS_Y,
                                 .w = HEALTH_ICON_WIDTH,
                                 .h = HEALTH_ICON_HEIGHT};
    asset_t *health_icon = asset_make_image(HEALTH_ICON_PATH, health_icon_rect);
    list_add(health_bar, health_icon);
  }
  return health_bar;
}
void render_health_bar(list_t *health_bar, size_t health) {
  for (size_t i = 0; i < health; i++) {
    asset_render(list_get(health_bar, i));
  }
}
void render_no_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                    ui_buttons_text_t *ui_buttons_text) {
  asset_render(ui_buttons->open_menu_button);
  asset_render(ui_buttons->shop_button);
}

void render_menu_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                      ui_buttons_text_t *ui_buttons_text) {
  asset_render(ui_pages->menu_page);
  menu_buttons_text_render(ui_buttons_text);
  asset_render(ui_buttons->quit_game_button);
  asset_render(ui_buttons->settings_button);
  asset_render(ui_buttons->help_button);
  asset_render(ui_buttons->close_menu_button);
}

void render_settings_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                          ui_buttons_text_t *ui_buttons_text) {
  asset_render(ui_pages->settings_page);
  settings_buttons_text_render(ui_buttons_text);
  asset_render(ui_buttons->back_to_menu_button);
  asset_render(ui_buttons->increase_music_volume_button);
  asset_render(ui_buttons->decrease_music_volume_button);
  asset_render(ui_buttons->increase_sound_effects_volume_button);
  asset_render(ui_buttons->decrease_sound_effects_volume_button);
  asset_render(ui_buttons->change_up_key_button);
  asset_render(ui_buttons->change_down_key_button);
  asset_render(ui_buttons->change_left_key_button);
  asset_render(ui_buttons->change_right_key_button);
  asset_render(ui_buttons->change_shoot_key_button);
}

void render_help_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                      ui_buttons_text_t *ui_buttons_text) {
  asset_render(ui_pages->help_page);
  asset_render(ui_buttons->back_to_menu_button);
}

void render_shop_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                      ui_buttons_text_t *ui_buttons_text) {
  asset_render(ui_pages->shop_page);
  asset_render(ui_buttons->back_to_menu_button);
  asset_render(ui_buttons->shop_upgrade1_button);
  asset_render(ui_buttons->shop_upgrade2_button);
  asset_render(ui_buttons->shop_upgrade3_button);
  shop_buttons_text_render(ui_buttons_text);
}
const char *ENEMIES_COUNT_TEXT_BEGIN = "Enemies left: ";
const double ENEMIES_COUNT_TEXT_WIDTH = MENU_TEXT_WIDTH;
const double ENEMIES_COUNT_TEXT_HEIGHT = MENU_TEXT_HEIGHT;
const double ENEMIES_COUNT_TEXT_X = 0;
const double ENEMIES_COUNT_TEXT_Y =
    SDL_SCREEN_DIM.y - ENEMIES_COUNT_TEXT_HEIGHT;
SDL_Rect enemies_count_text_rect = {.x = ENEMIES_COUNT_TEXT_X,
                                    .y = ENEMIES_COUNT_TEXT_Y,
                                    .w = ENEMIES_COUNT_TEXT_WIDTH,
                                    .h = ENEMIES_COUNT_TEXT_HEIGHT};
asset_t *init_enemies_count(const char *font_filepath, rgb_color_t color) {
  return asset_make_text(font_filepath, enemies_count_text_rect,
                         ENEMIES_COUNT_TEXT_BEGIN, color);
}
const size_t MAX_ENEMIES_COUNT = 100;
asset_t *update_enemies_count_text(asset_t *old_text, const char *font_filepath,
                                   rgb_color_t color, size_t enemies_count) {
  asset_destroy(old_text);
  char enemies_count_to_string[MAX_ENEMIES_COUNT];
  sprintf(enemies_count_to_string, "%zu", enemies_count);
  char *enemies_count_text = malloc(strlen(ENEMIES_COUNT_TEXT_BEGIN) +
                                    strlen(enemies_count_to_string) + 1);
  strcpy(enemies_count_text, ENEMIES_COUNT_TEXT_BEGIN);
  strcat(enemies_count_text, enemies_count_to_string);
  return asset_make_text(font_filepath, enemies_count_text_rect,
                         enemies_count_text, color);
}
const size_t MAX_LEVEL_COUNT = 5;
const char *LEVEL_COUNT_TEXT_BEGIN = "Current level is ";
asset_t *update_level_text(asset_t *old_text, const char *font_filepath,
                           rgb_color_t color, size_t level) {
  asset_destroy(old_text);
  char level_count_to_string[MAX_LEVEL_COUNT];
  sprintf(level_count_to_string, "%zu", level);
  char *level_count_text = malloc(strlen(LEVEL_COUNT_TEXT_BEGIN) +
                                  strlen(level_count_to_string) + 1);
  strcpy(level_count_text, LEVEL_COUNT_TEXT_BEGIN);
  strcat(level_count_text, level_count_to_string);
  return asset_make_text(font_filepath, current_level_text_rect,
                         level_count_text, color);
}
void render_level_update_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                              ui_buttons_text_t *ui_buttons_text) {
  asset_render(ui_pages->level_update_page);
}
