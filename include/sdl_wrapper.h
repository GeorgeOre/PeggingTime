#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "body.h"
#include "color.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "state.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

// Values passed to a key handler when the given arrow key is pressed
typedef enum {
  LEFT_ARROW = 1,
  UP_ARROW = 2,
  RIGHT_ARROW = 3,
  DOWN_ARROW = 4,
  SPACE_BAR = 5,
} arrow_key_t;

/**
 * The possible types of key events.
 * Enum types in C are much more primitive than in Java; this is equivalent to:
 * typedef unsigned int KeyEventType;
 * #define KEY_PRESSED 0
 * #define KEY_RELEASED 1
 */
typedef enum { KEY_PRESSED, KEY_RELEASED } key_event_type_t;

/**
 * A keypress handler.
 * When a key is pressed or released, the handler is passed its char value.
 * Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
 * Arrow keys have the special values listed above.
 *
 * @param key a character indicating which key was pressed
 * @param type the type of key event (KEY_PRESSED or KEY_RELEASED)
 * @param held_time if a press event, the time the key has been held in seconds
 */
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time,
                              void *state);

/**
 * A mouse click handler.
 * When a mouse press is detected, the handler is called.
 *
 * @param xcor the x position of the mouse when it was clicked
 * @param ycor the y position of the mouse when it was clicked
 */
typedef void (*mouse_handler_t)(void *state, double xcor, double ycor);

/**
 * Initializes the SDL window and renderer.
 * Must be called once before any of the other SDL functions.
 *
 * @param min the x and y coordinates of the bottom left of the scene
 * @param max the x and y coordinates of the top right of the scene
 */
void sdl_init(vector_t min, vector_t max);

/**
 * Processes all SDL events and returns whether the window has been closed.
 * This function must be called in order to handle inputs.
 *
 * @return true if the window was closed, false otherwise
 */
bool sdl_is_done(void *state);

/**
 * Clears the screen. Should be called before drawing polygons in each frame.
 */
void sdl_clear(void);

/**
 * Creates an SDL_Rect with the specified dimensions at the specified position.
 *
 * @param width the width of the rect
 * @param height the height of the rect
 * @param xpos the x location of the rect with + x defined rightwards of screen
 * @param ypos the y location of the rect with + y defined topwards of screen
 */
SDL_Rect *sdl_init_rect(int width, int height, int xpos, int ypos);

/**
 * Creates an SDL_Texture (image) from the specified filepath. The returned
 * NOTE: SDL_Texture must be freed with SDL_DestroyTexture().
 *
 * @param filepath the name of the image file to render
 * @return the SDL_Texture object
 */
SDL_Texture *sdl_init_texr(char *filepath);

/**
 * Stores the height and width of the specified SDL_Texture in the height and
 * width arguments.
 * NOTE: The height and width arguments must be freed using free().
 *
 * @param texr the SDL_Texture
 * @param height the height of the texture. Must be freed.
 * @param width the width of the texture. Must be freed
 */
void sdl_get_texr_dimensions(SDL_Texture *texr, int *height, int *width);

/**
 * Renders the specified SDL_Texture on a SDL_Rect but rotated.
 *
 * @param texr the SDL_Texture to render
 * @param rect the SDL_Rect to render to
 * @param angle angle to rotate the texture
 * @param pos position to rotate about
 */
void sdl_render_rotate_texr(SDL_Texture *texr, SDL_Rect *rect, double angle,
                            vector_t pos);

/**
 * Renders the specified SDL_Texture on a SDL_Rect.
 *
 * @param texr the SDL_Texture to render
 * @param rect the SDL_Rect to render to
 */
void sdl_render_texr_offscreen(SDL_Texture *texr, SDL_Rect *rect);

/**
 * Returns a TTF_Font object from the specified filepath.
 * NOTE: The TTF_Font object should be freed using TTF_CloseFont().
 *
 * @param filepath the location of the font
 * @param ptsize the font size
 * @return TTF_Font object representing the desired font.
 */
TTF_Font *sdl_init_font(char *filepath, int ptsize);

/**
 * Renders the text in the specified font and color.
 *
 * @param text the text to render
 * @param font the TTF_Font object of the font to use
 * @param color the RGB color to render the font in
 * @param rect the SDL_Rect to render to
 */
void sdl_render_text_offscreen(char *text, TTF_Font *font, SDL_Rect *rect,
                               rgb_color_t color);

/**
 * Draws a polygon from the given list of vertices and a color.
 *
 * @param poly a struct representing the polygon
 * @param color the color used to fill in the polygon
 */
void sdl_draw_polygon(polygon_t *poly, rgb_color_t color);

/**
 * Displays the rendered frame on the SDL window.
 * Must be called after drawing the polygons in order to show them.
 */
void sdl_show(void);

/**
 * Draws all bodies in a scene.
 * This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show(),
 * so those functions should not be called directly.
 *
 * @param scene the scene to draw
 * @param aux an additional body to draw (can be NULL if no additional bodies)
 */
void sdl_render_scene(scene_t *scene, void *aux);

/**
 * Registers a function to be called every time a key is pressed.
 * Overwrites any existing handler.
 *
 * Example:
 * ```
 * void on_key(char key, key_event_type_t type, double held_time) {
 *     if (type == KEY_PRESSED) {
 *         switch (key) {
 *             case 'a':
 *                 printf("A pressed\n");
 *                 break;
 *             case UP_ARROW:
 *                 printf("UP pressed\n");
 *                 break;
 *         }
 *     }
 * }
 * int main(void) {
 *     sdl_on_key(on_key);
 *     while (!sdl_is_done());
 * }
 * ```
 *
 * @param handler the function to call with each key press
 */
void sdl_on_key(key_handler_t handler);

/**
 * Registers a function to be called everytime the mouse is clicked.
 *
 * @param handler the handler function to call.
 */
void sdl_on_click(mouse_handler_t handler);

/**
 * Gets the amount of time that has passed since the last time
 * this function was called, in seconds.
 *
 * @return the number of seconds that have elapsed
 */
double time_since_last_tick(void);

// The sdl rectangle bounding a body.
SDL_Rect *bounding_box(body_t *body);

/**
 * Initializes the SDL Mixer system for WAV input.
 *
 * @return true if the SDL Mixer successfully initialized.
 */
bool sdl_init_mixer();

/**
 * Closes the SDL Mixer system.
 *
 */
void sdl_close_mixer();

/**
 * Loads the music from the specified filepath.
 * NOTE:The music file should be a .WAV.
 *
 * @param filepath the filepath of the music
 * @return the pointer to SDL Mix_Music struct of the music
 */
Mix_Music *sdl_mixer_load_music(char *filepath);

/**
 * Plays the music loaded in the SDL Mixer. This function should only be called
 * once to start the music playback.
 *
 * @param music the SDL Mix_Music structure to play
 */
void sdl_mixer_play_music(Mix_Music *music);

/**
 * Pauses the current music being played through the SDL Mixer.
 */
void sdl_mixer_pause_music();

/**
 * Sets the SDL Mixer Music channel volume. The specified volume should be
 * a value between 0 and 1. Any value out of range will be clamped.
 *
 * @param volume the volume value, between 0 and 1
 */
void sdl_mixer_set_music_volume(double volume);

/**
 * Gets the SDL Mixer Music channel volume as a value between 0 and 1.
 *
 * @return volume the SDL Mixer Music volume
 */
double sdl_mixer_get_music_volume();

/**
 * Resumes the music being played through the SDL Mixer.
 */
void sdl_mixer_resume_music();

/**
 * Frees the music being played through the SDL Mixer.
 * NOTE: If music is being played, this function will stop the music first
 * and then free it.
 *
 * @param music the SDL Mix_Music structure to free
 */
void sdl_mixer_free_music(Mix_Music *music);

/**
 * Loads the sound effect from the specified filepath on the SDL Mixer Chunk
 * channels.
 * NOTE:The music file should be a .WAV.
 *
 * @param filepath the filepath of the music
 * @return the pointer to SDL Mix_Music struct of the music
 */
Mix_Chunk *sdl_mixer_load_sound_effect(char *filepath);

/**
 * Plays the sound effect through the SDL Mixer Chunk channels
 * NOTE: If all channels are currently used, the sound effect will not be
 * played.
 * @param sound_effect the sound effect to play as a Mix_Chunk
 */
void sdl_mixer_play_sound_effect(Mix_Chunk *sound_effect);

/**
 * Sets the volume of all SDL Mixer Chunk channels. The specified volume should
 * be a value between 0 and 1. Any value out of range will be clamped.
 *
 * @param volume the volume value, between 0 and 1
 */
void sdl_mixer_set_sound_effects_volume(double volume);

/**
 * Gets the volume of all SDL Mixer Chunk channels as a value between 0 and 1.
 *
 * @return the volume value between 0 and 1
 */
double sdl_mixer_get_sound_effects_volume();

/**
 * Frees the specified SDL Mixer Mix_Chunk sound effect
 */
void sdl_mixer_free_sound_effect(Mix_Chunk *sound_effect);

/**
 * Resets the volume of SDL Mixer Music and Chunk channels to the DEFAULT_VOLUME
 * of 0.5.
 */
void sdl_mixer_reset_volume_all();
#endif // #ifndef __SDL_WRAPPER_H__
