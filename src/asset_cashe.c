#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "list.h"
#include "sdl_wrapper.h"

#include "string.h"

static list_t *ASSET_CACHE;

const size_t FONT_SIZE = 18;
const size_t INITIAL_CAPACITY = 5;

typedef struct {
  asset_type_t type;
  const char *filepath;
  void *obj;
} entry_t;

static void asset_cache_free_entry(entry_t *entry) {
  // free the object the asset holds
  // use switch case in case more types are added
  switch (entry->type) {
  case ASSET_IMAGE:
    free((char *)(entry->filepath));
    SDL_DestroyTexture(entry->obj);
  case ASSET_FONT:
    free((char *)(entry->filepath));
    TTF_CloseFont(entry->obj);
  case ASSET_BUTTON:
    asset_destroy(entry->obj);
  }
  free(entry);
}

void asset_cache_init() {
  ASSET_CACHE =
      list_init(INITIAL_CAPACITY, (free_func_t)asset_cache_free_entry);
}

void asset_cache_destroy() { list_free(ASSET_CACHE); }

void *asset_cache_obj_get_or_create(asset_type_t ty, const char *filepath) {
  size_t cache_size = list_size(ASSET_CACHE);
  entry_t *entry;
  for (size_t i = 0; i < cache_size; i++) {
    entry = list_get(ASSET_CACHE, i);
    if (entry->filepath != NULL &&
        strcmp((char *)(entry->filepath), (char *)filepath)) {
      assert(entry->type == ty);
      return entry->obj;
    }
  }
  // filepath not found, need to make new object
  entry = malloc(sizeof(entry_t));
  assert(entry != NULL);
  entry->type = ty;
  // COPY the filepath string
  entry->filepath = malloc(strlen(filepath) * sizeof(char) + 1);
  assert(entry->filepath != NULL);
  strcpy((char *)entry->filepath, filepath);
  // create the new object depending on the type.
  // Use switch case in case more asset types are added.
  switch (ty) {
  case ASSET_IMAGE:
    entry->obj = sdl_init_texr((char *)filepath);
    return entry->obj;
  case ASSET_FONT:
    entry->obj = sdl_init_font((char *)filepath, FONT_SIZE);
    return entry->obj;
  default:
    return NULL;
  }
}

void asset_cache_register_button(asset_t *button) {
  // Assert that the button is ASSET_BUTTON type
  assert(asset_get_type(button) == ASSET_BUTTON);

  // Check to see if the the button is already registered
  bool registered = false;
  entry_t *entry;
  size_t cache_size = list_size(ASSET_CACHE);
  for (size_t i = 0; i < cache_size; i++) {
    entry = list_get(ASSET_CACHE, i);
    // If the button is found, record it in the bool var
    if (entry->obj == button) {
      registered = true;
    }
  }

  // If the button was not found in the cache, add it
  if (registered == false) {
    // Wrap the button in and entry_t type
    entry = malloc(sizeof(entry_t));
    assert(entry != NULL);
    entry->type = ASSET_BUTTON;
    entry->obj = button;
    entry->filepath = NULL;
    list_add(ASSET_CACHE, entry);
  }
}

void asset_cache_handle_buttons(state_t *state, double x, double y) {
  // Iterate the asset cache for all buttons
  size_t cache_size = list_size(ASSET_CACHE);
  entry_t *entry;
  for (size_t i = 0; i < cache_size; i++) {
    entry = list_get(ASSET_CACHE, i);
    // If a button is found, handle it
    if (entry->type == ASSET_BUTTON) {
      asset_on_button_click(entry->obj, state, x, y);
    }
  }
}
