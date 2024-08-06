#ifndef __UI_H__
#define __UI_H__

#include "asset.h"
#include "list.h"

/** @author Steven Lei*/

// Define what "page", which is essentially are subscreens for the
// userinterface, that should be should be displayed
typedef enum {
  NO_PAGE,
  MENU_PAGE,
  SETTINGS_PAGE,
  HELP_PAGE,
  SHOP_PAGE,
  LEVEL_UPDATE_PAGE
} ui_page_state;

// Define what screen should be displayed
typedef enum { LOAD_SCREEN, GAME_SCREEN } screen_state;
/**
 * Defines the type of key that is pressed based on its function in the game
 */
typedef enum {
  NONE,
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
  SHOOT,
} key_type;

/**
 * Contains all keybinds that were configured by the user.
 */
typedef struct keybinds {
  char MOVE_LEFT;
  char MOVE_RIGHT;
  char MOVE_UP;
  char MOVE_DOWN;
  char SHOOT;
} keybinds_t;

/**
 * Contains the relevant information to create a button asset using an image
 * or text.
 */
typedef struct button_info {
  const char *image_path;
  const char *font_path;
  SDL_Rect image_box;
  SDL_Rect text_box;
  rgb_color_t text_color;
  const char *text;
  button_handler_t handler;
} button_info_t;

/**
 * Contains all the user interface buttons rendered in the game.
 */
typedef struct ui_buttons {
  asset_t *open_menu_button;
  asset_t *close_menu_button;
  asset_t *settings_button;
  asset_t *help_button;
  asset_t *back_to_menu_button;
  asset_t *quit_game_button;
  asset_t *play_game_button;
  asset_t *increase_music_volume_button;
  asset_t *decrease_music_volume_button;
  asset_t *increase_sound_effects_volume_button;
  asset_t *decrease_sound_effects_volume_button;
  asset_t *change_left_key_button;
  asset_t *change_right_key_button;
  asset_t *change_up_key_button;
  asset_t *change_down_key_button;
  asset_t *change_shoot_key_button;
  asset_t *shop_button;
  asset_t *shop_upgrade1_button;
  asset_t *shop_upgrade2_button;
  asset_t *shop_upgrade3_button;
} ui_buttons_t;

/**
 * Contains all the information for creating all the user interface buttons.
 */
typedef struct ui_buttons_info {
  button_info_t open_menu_button_info;
  button_info_t close_menu_button_info;
  button_info_t settings_button_info;
  button_info_t help_button_info;
  button_info_t back_to_menu_button_info;
  button_info_t quit_game_button_info;
  button_info_t play_game_button_info;
  button_info_t increase_music_volume_button_info;
  button_info_t decrease_music_volume_button_info;
  button_info_t increase_sound_effects_volume_button_info;
  button_info_t decrease_sound_effects_volume_button_info;
  button_info_t change_left_key_button_info;
  button_info_t change_right_key_button_info;
  button_info_t change_up_key_button_info;
  button_info_t change_down_key_button_info;
  button_info_t change_shoot_key_button_info;
  button_info_t shop_button_info;
  button_info_t shop_upgrade1_button_info;
  button_info_t shop_upgrade2_button_info;
  button_info_t shop_upgrade3_button_info;
} ui_buttons_info_t;

/**
 * Contains all "pages" rendered in the game, which is essentially a subscreen
 * used in the menu and shop interfaces.
 */
typedef struct ui_pages {
  asset_t *menu_page;
  asset_t *settings_page;
  asset_t *help_page;
  asset_t *shop_page;
  asset_t *level_update_page;
} ui_pages_t;

/**
 * Contains all relevant text that will be rendered in the user interface pages
 */
typedef struct ui_buttons_text {
  asset_t *close_menu_button_text;
  asset_t *settings_button_text;
  asset_t *help_button_text;
  asset_t *quit_game_button_text;
  asset_t *music_volume_buttons_text;
  asset_t *sound_effects_volume_buttons_text;
  asset_t *key_binds_info_text1;
  asset_t *key_binds_info_text2;
  asset_t *up_key_info_text;
  asset_t *down_key_info_text;
  asset_t *left_key_info_text;
  asset_t *right_key_info_text;
  asset_t *shoot_key_info_text;
  asset_t *shop_info_text;
  asset_t *shop_upgrade1_button_text;
  asset_t *shop_upgrade2_button_text;
  asset_t *shop_upgrade3_button_text;
  asset_t *current_level_text;
} ui_buttons_text_t;

/**
 * Create a button asset from the provided info.
 *
 * @param button_assets a list of the assets the button will use
 * @param button_info the information provided to create the button
 *
 * @return the button asset
 */
asset_t *create_button_from_info(list_t *button_assets,
                                 button_info_t button_info);

/**
 * Creates all the user interface buttons in the specified ui_buttons structure
 * as assets.
 *
 * @param button_assets a list of the assets the buttonn wil use
 * @param ui_buttons_info all of the button information needed for the ui
 *
 * @return the ui_buttons structure containing all buttons as assets
 */
ui_buttons_t *create_ui_buttons(list_t *button_assets,
                                ui_buttons_info_t ui_buttons_info);
/**
 * Gets all info for every user interface button
 *
 * @return the ui_button_info structure containing information of every
 * user interface button
 */
ui_buttons_info_t get_ui_buttons_info();

/**
 * Frees all the buttons contained in the ui_buttons structure
 *
 * @param ui_buttons all buttons used for the user interface
 */
void ui_buttons_free(ui_buttons_t *ui_buttons);

/**
 * Creates all user interface pages
 *
 * @return the ui_pages structure containing all interface pages
 */

ui_pages_t *create_ui_pages();

/**
 * Frees all user interface pages
 * @param pages the pages to free
 */
void ui_pages_free(ui_pages_t *pages);

/**
 * Creates all relevant text needed to be rendered next to buttons in a
 * user interface page.
 *
 * @param font_filepath the path to the font to render the text in
 * @param color the color to render the text in
 *
 * @return the ui_buttons structure containing all text assets to be rendered
 */
ui_buttons_text_t *create_ui_buttons_text(const char *font_filepath,
                                          rgb_color_t color);
/**
 * Renders all text relevant to the menu buttons
 * @param buttons_text the ui_buttons structure containing the text assets to
 * render
 */
void menu_buttons_text_render(ui_buttons_text_t *buttons_text);
/**
 * Renders all text relevant to settings buttons
 *
 * @param buttons_text the ui_buttons structure containing the text assets to
 * render
 */
void shop_buttons_text_render(ui_buttons_text_t *buttons_text);
/**
 * Renders all text relevant to the shop buttons
 *
 * @param buttons_text the ui_buttons structure containing the text assets to
 * render
 */
void settings_buttons_text_render(ui_buttons_text_t *buttons_text);
/**
 * Frees all the text assets associated with the user interface
 *
 * @param buttons_text the ui_buttons_text strcuture containing all the text
 * assets
 */
void ui_buttons_text_free(ui_buttons_text_t *buttons_text);
/**
 * Updates the keybinds text that shows the current players's keybinds
 * @param buttons_text the ui_buttons structure containing the text to update
 * @param keybinds the player's current keybinds
 * @param font_filepath the font to render the text in
 * @param color the color to render the text in
 */
void update_key_binds_text(ui_buttons_text_t *buttons_text, keybinds_t keybinds,
                           const char *font_filepath, rgb_color_t color);
/**
 *  Updates the shop information text that shows the current players's gold
 * @param buttons_text the ui_buttons structure containing the text to update
 * @param gold the player's current gold
 * @param font_filepath the font to render the text in
 * @param color the color to render the text in
 */
void update_shop_info_text(ui_buttons_text_t *buttons_text, size_t gold,
                           const char *font_filepath, rgb_color_t color);
/**
 * Initializes the health bar showing the player's current health
 * @param max_health the maximum health to display
 *
 * @return the health bar asset list containing the health point assets
 */
list_t *init_health_bar(size_t max_health);

/**
 * Renders the health bar showing the player's current health
 * @param health_bar the health bar to render
 * @param health the amount of health points to render
 */

void render_health_bar(list_t *health_bar, size_t health);

/**
 * Renders the associated assets when no page is displayed
 * @param ui_pages the ui_page structure containing the pages to render
 * @param ui_buttons the ui_button structure containing the buttons to render
 * @param ui_buttons_text the ui_button_text structure containing text to render
 */
void render_no_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                    ui_buttons_text_t *ui_buttons_text);

/**
 * Renders the associated assets when the menu page is displayed
 * @param ui_pages the ui_page structure containing the pages to render
 * @param ui_buttons the ui_button structure containing the buttons to render
 * @param ui_buttons_text the ui_button_text structure containing text to render
 */
void render_menu_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                      ui_buttons_text_t *ui_buttons_text);

/**
 * Renders the associated assets when the settings page is displayed
 * @param ui_pages the ui_page structure containing the pages to render
 * @param ui_buttons the ui_button structure containing the buttons to render
 * @param ui_buttons_text the ui_button_text structure containing text to render
 */
void render_settings_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                          ui_buttons_text_t *ui_buttons_text);

/**
 * Renders the associated assets when help page is displayed
 * @param ui_pages the ui_page structure containing the pages to render
 * @param ui_buttons the ui_button structure containing the buttons to render
 * @param ui_buttons_text the ui_button_text structure containing text to render
 */
void render_help_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                      ui_buttons_text_t *ui_buttons_text);

/**
 * Renders the associated assets when the shop page is displayed
 * @param ui_pages the ui_page structure containing the pages to render
 * @param ui_buttons the ui_button structure containing the buttons to render
 * @param ui_buttons_text the ui_button_text structure containing text to render
 */
void render_shop_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                      ui_buttons_text_t *ui_buttons_text);

/**
 * Renders the associated assets when level udpate page is displayed
 * @param ui_pages the ui_page structure containing the pages to render
 * @param ui_buttons the ui_button structure containing the buttons to render
 * @param ui_buttons_text the ui_button_text structure containing text to render
 */
void render_level_update_page(ui_pages_t *ui_pages, ui_buttons_t *ui_buttons,
                              ui_buttons_text_t *ui_buttons_text);

asset_t *init_enemies_count(const char *font_filepath, rgb_color_t color);
asset_t *update_enemies_count_text(asset_t *old_text, const char *font_filepath,
                                   rgb_color_t color, size_t enemies_count);
#endif // #ifndef __UI_H__
