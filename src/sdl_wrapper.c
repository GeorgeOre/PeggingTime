#include "sdl_wrapper.h"
#include "asset_cache.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <vector.h>

const char WINDOW_TITLE[] = "CS 3";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const double MS_PER_S = 1e3;
const __int128_t MIXER_VOLUME_SCALE = 127; // SDL Mixer volume is 128 bit based
const double MIXER_MIN_VOLUME = 0.0;       // The min volume for the SDL Mixer
const double MIXER_MAX_VOLUME = 1.0;       // The max volume for the SDL Mixer
const double MIXER_DEFAULT_VOLUME = 0.5; // The default volume for the SDL Mixer

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;

/**
 * The mousepress handler, or NULL if none has been configured.
 */
mouse_handler_t mouse_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

clock_t total_clock = 0;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
  int *width = malloc(sizeof(*width)), *height = malloc(sizeof(*height));
  assert(width != NULL);
  assert(height != NULL);
  SDL_GetWindowSize(window, width, height);
  vector_t dimensions = {.x = *width, .y = *height};
  free(width);
  free(height);
  return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
  // Scale scene so it fits entirely in the window
  double x_scale = window_center.x / max_diff.x,
         y_scale = window_center.y / max_diff.y;
  return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
  // Scale scene coordinates by the scaling factor
  // and map the center of the scene to the center of the window
  vector_t scene_center_offset = vec_subtract(scene_pos, center);
  double scale = get_scene_scale(window_center);
  vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
  vector_t pixel = {.x = round(window_center.x + pixel_center_offset.x),
                    // Flip y axis since positive y is down on the screen
                    .y = round(window_center.y - pixel_center_offset.y)};
  return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
  switch (key) {
  case SDLK_LEFT:
    return LEFT_ARROW;
  case SDLK_UP:
    return UP_ARROW;
  case SDLK_RIGHT:
    return RIGHT_ARROW;
  case SDLK_DOWN:
    return DOWN_ARROW;
  case SDLK_SPACE:
    return SPACE_BAR;
  default:
    // Only process 7-bit ASCII characters
    return key == (SDL_Keycode)(char)key ? key : '\0';
  }
}

void sdl_init(vector_t min, vector_t max) {
  // Check parameters
  assert(min.x < max.x);
  assert(min.y < max.y);

  center = vec_multiply(0.5, vec_add(min, max));
  max_diff = vec_subtract(max, center);
  SDL_Init(SDL_INIT_EVERYTHING);
  window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                            SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  TTF_Init();
  sdl_init_mixer();
}

bool sdl_is_done(void *state) {
  SDL_Event *event = malloc(sizeof(*event));
  assert(event != NULL);
  while (SDL_PollEvent(event)) {
    switch (event->type) {
    case SDL_QUIT:
      sdl_close_mixer();
      free(event);
      return true;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      // Skip the keypress if no handler is configured
      // or an unrecognized key was pressed
      if (key_handler == NULL)
        break;
      char key = get_keycode(event->key.keysym.sym);
      if (key == '\0')
        break;

      uint32_t timestamp = event->key.timestamp;
      if (!event->key.repeat) {
        key_start_timestamp = timestamp;
      }
      key_event_type_t type =
          event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
      double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
      key_handler(key, type, held_time, state);
      break;
    case SDL_MOUSEBUTTONUP: {
      int x, y;
      SDL_GetMouseState(&x, &y);
      asset_cache_handle_buttons(state, x, y);
    }
    }
  }
  free(event);
  return false;
}

void sdl_clear(void) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);
}

SDL_Rect *sdl_init_rect(int width, int height, int xpos, int ypos) {
  SDL_Rect *rect = malloc(sizeof(SDL_Rect));
  assert(rect != NULL);
  rect->w = width;
  rect->h = height;
  rect->x = xpos;
  rect->y = ypos;
  return rect;
}

SDL_Texture *sdl_init_texr(char *filepath) {
  SDL_Texture *texr = IMG_LoadTexture(renderer, filepath);
  // if file not found, the texture will not render
  assert(texr != NULL);
  return texr;
}

void sdl_get_texr_dimensions(SDL_Texture *texr, int *height, int *width) {
  // Note: height and width must be freed by the user!
  height = malloc(sizeof(int));
  assert(height != NULL);
  width = malloc(sizeof(int));
  assert(width != NULL);
  SDL_QueryTexture(texr, NULL, NULL, width, height);
}

void sdl_render_rotate_texr(SDL_Texture *texr, SDL_Rect *rect, double angle,
                            vector_t pos) {
  //   SDL_Point rot_point = {(int)pos.x, (int)pos.y};
  //   SDL_RenderCopyEx(renderer, texr, NULL, rect, angle, &rot_point,
  //                    SDL_FLIP_NONE);
  SDL_RenderCopyEx(renderer, texr, NULL, rect, angle, NULL, SDL_FLIP_NONE);
}

void sdl_render_texr_offscreen(SDL_Texture *texr, SDL_Rect *rect) {
  SDL_RenderCopy(renderer, texr, NULL, rect);
}

TTF_Font *sdl_init_font(char *filepath, int ptsize) {
  TTF_Font *font = TTF_OpenFont(filepath, ptsize);
  if (font == NULL) {
    printf("File path is %s, with SDL Error: %s\n", filepath, SDL_GetError());
  }
  assert(font);
  return font;
}

void sdl_render_text_offscreen(char *text, TTF_Font *font, SDL_Rect *rect,
                               rgb_color_t color) {
  SDL_Color sdl_color = {color.r, color.g, color.b};
  SDL_Surface *message_surface = TTF_RenderText_Solid(font, text, sdl_color);
  assert(message_surface != NULL);
  SDL_Texture *message_texture =
      SDL_CreateTextureFromSurface(renderer, message_surface);
  assert(message_texture != NULL);
  SDL_RenderCopy(renderer, message_texture, NULL, rect);
  // frees the surface and texture, no longer used
  SDL_FreeSurface(message_surface);
  SDL_DestroyTexture(message_texture);
}

void sdl_draw_polygon(polygon_t *poly, rgb_color_t color) {
  list_t *points = polygon_get_points(poly);
  // Check parameters
  size_t n = list_size(points);
  assert(n >= 3);
  assert(0 <= color.r && color.r <= 1);
  assert(0 <= color.g && color.g <= 1);
  assert(0 <= color.b && color.b <= 1);

  vector_t window_center = get_window_center();

  // Convert each vertex to a point on screen
  int16_t *x_points = malloc(sizeof(*x_points) * n),
          *y_points = malloc(sizeof(*y_points) * n);
  assert(x_points != NULL);
  assert(y_points != NULL);
  for (size_t i = 0; i < n; i++) {
    vector_t *vertex = list_get(points, i);
    vector_t pixel = get_window_position(*vertex, window_center);
    x_points[i] = pixel.x;
    y_points[i] = pixel.y;
  }

  // Draw polygon with the given color
  filledPolygonRGBA(renderer, x_points, y_points, n, color.r * 255,
                    color.g * 255, color.b * 255, 255);

  total_clock = clock();
  double secs = total_clock ? (double)(total_clock) / CLOCKS_PER_SEC
                            : 0.0; // return 0 the first time this is called

  if (secs > 10) {
    size_t wait = CLOCKS_PER_SEC * 3;
    while (wait >= 0) {
      /* INFINEITE */
      SDL_RenderPresent(renderer);
      printf("%zu\n", wait);
      wait--;
    }
  }

  free(x_points);
  free(y_points);
}

void sdl_show(void) {
  // Draw boundary lines
  vector_t window_center = get_window_center();
  vector_t max = vec_add(center, max_diff),
           min = vec_subtract(center, max_diff);
  vector_t max_pixel = get_window_position(max, window_center),
           min_pixel = get_window_position(min, window_center);
  SDL_Rect *boundary = malloc(sizeof(*boundary));
  boundary->x = min_pixel.x;
  boundary->y = max_pixel.y;
  boundary->w = max_pixel.x - min_pixel.x;
  boundary->h = min_pixel.y - max_pixel.y;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderDrawRect(renderer, boundary);
  free(boundary);
  SDL_RenderPresent(renderer);
}

void sdl_render_scene(scene_t *scene, void *aux) {
  size_t body_count = scene_bodies(scene);
  for (size_t i = 0; i < body_count; i++) {
    body_t *body = scene_get_body(scene, i);
    list_t *shape = body_get_shape(body);
    polygon_t *poly = polygon_init(shape, (vector_t){0, 0}, 0, 0, 0, 0);
    sdl_draw_polygon(poly, *body_get_color(body));
    list_free(shape);
  }
  if (aux != NULL) {
    body_t *body = aux;
    sdl_draw_polygon(body_get_polygon(body), *body_get_color(body));
  }
}

void sdl_on_key(key_handler_t handler) { key_handler = handler; }

void sdl_on_click(mouse_handler_t handler) { mouse_handler = handler; }

double time_since_last_tick(void) {
  clock_t now = clock();
  double difference = last_clock
                          ? (double)(now - last_clock) / CLOCKS_PER_SEC
                          : 0.0; // return 0 the first time this is called
  last_clock = now;
  return difference;
}

SDL_Rect *bounding_box(body_t *body) {
  // box will hold the bounding box and be returned.
  SDL_Rect *box;
  // points will hold the vertices of the body.
  list_t *points;

  double temp_x;
  double temp_y;

  double min_x;
  double max_y;
  double max_x;
  double min_y;

  min_x = __DBL_MAX__;
  max_y = -__DBL_MAX__;
  max_x = -__DBL_MAX__;
  min_y = __DBL_MAX__;

  // Loop through body vertices to find the min x and max y and max x and min
  // y.
  points = body_get_shape(body);
  for (size_t i = 0; i < list_size(points); i++) {
    temp_x = ((vector_t *)list_get(points, i))->x;
    temp_y = ((vector_t *)list_get(points, i))->y;
    if (temp_x < min_x) {
      min_x = temp_x;
    }
    if (temp_y > max_y) {
      max_y = temp_y;
    }
    if (temp_x > max_x) {
      max_x = temp_x;
    }
    if (temp_y < min_y) {
      min_y = temp_y;
    }
  }

  // Fill in box.
  box = malloc(sizeof(SDL_Rect));
  box->x = min_x;
  box->y = WINDOW_HEIGHT - max_y;
  box->w = max_x - min_x;
  box->h = max_y - min_y;

  return box;
}

// The volume of the SDL Mixer Music channel
double sdl_mixer_music_volume = MIXER_DEFAULT_VOLUME;

// The Volume of the SDL Mixer Chunks channel. Reserved for sound effects
double sdl_mixer_sound_effects_volume = MIXER_DEFAULT_VOLUME;

bool sdl_init_mixer() {
  assert(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != -1);
  sdl_mixer_reset_volume_all();
  return true;
}

void sdl_close_mixer() { Mix_CloseAudio(); }

Mix_Music *sdl_mixer_load_music(char *filepath) {
  Mix_Music *music = Mix_LoadMUS((const char *)filepath);
  assert(music != NULL);
  return music;
}

void sdl_mixer_play_music(Mix_Music *music) { Mix_PlayMusic(music, -1); }

void sdl_mixer_pause_music() { Mix_PauseMusic(); }

void sdl_mixer_resume_music() { Mix_ResumeMusic(); }

void sdl_mixer_set_music_volume(double volume) {
  // Clamp the volume
  if (volume > MIXER_MAX_VOLUME) {
    volume = MIXER_MAX_VOLUME;
  }
  if (volume <= MIXER_MIN_VOLUME) {
    volume = MIXER_MIN_VOLUME;
  }
  sdl_mixer_music_volume = volume;
  Mix_VolumeMusic(volume * MIXER_VOLUME_SCALE);
}

double sdl_mixer_get_music_volume() { return sdl_mixer_music_volume; }

void sdl_mixer_free_music(Mix_Music *music) {
  // Halt the music if it is still playing.
  Mix_HaltMusic();
  Mix_FreeMusic(music);
}

Mix_Chunk *sdl_mixer_load_sound_effect(char *filepath) {
  Mix_Chunk *sound = Mix_LoadWAV((const char *)filepath);
  assert(sound);
  return sound;
}

void sdl_mixer_play_sound_effect(Mix_Chunk *sound_chuck) {
  Mix_PlayChannel(-1, sound_chuck, 1);
}

void sdl_mixer_set_sound_effects_volume(double volume) {
  if (volume > MIXER_MAX_VOLUME) {
    volume = MIXER_MAX_VOLUME;
  }
  if (volume <= MIXER_MIN_VOLUME) {
    volume = MIXER_MIN_VOLUME;
  }
  sdl_mixer_sound_effects_volume = volume;
  Mix_MasterVolume(volume * MIXER_VOLUME_SCALE);
}

double sdl_mixer_get_sound_effects_volume() {
  return sdl_mixer_sound_effects_volume;
}
void sdl_mixer_free_sound_effect(Mix_Chunk *sound_effect) {
  Mix_FreeChunk(sound_effect);
}
void sdl_mixer_reset_volume_all() {
  sdl_mixer_set_music_volume(MIXER_DEFAULT_VOLUME);
  sdl_mixer_set_sound_effects_volume(MIXER_DEFAULT_VOLUME);
}
