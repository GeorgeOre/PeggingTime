#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#include "asset.h"
#include "asset_cache.h"
#include "color.h"
#include "sdl_wrapper.h"
#include "shape.h"
#include "state.h"

const asset_type_t ASSET_BACKGROUND = 3;

struct asset {
  asset_type_t type;
  SDL_Rect bounding_box;
};

typedef struct text_asset {
  asset_t base;
  TTF_Font *font;
  const char *text;
  rgb_color_t color;
} text_asset_t;

typedef struct image_asset {
  asset_t base;
  SDL_Texture *texture;
  body_t *body;
} image_asset_t;

typedef struct button_asset {
  asset_t base;
  image_asset_t *image_asset;
  text_asset_t *text_asset;
  button_handler_t handler;
  bool is_rendered;
} button_asset_t;

// THIS SHOULD HOLD ANIMATIONS AND SPRITES AND SUCH
typedef struct ai_asset {
  asset_t base; // To enable "polymorphism"
  SDL_Rect bounding_box;
  bool is_rendered;
} ai_asset_t;

/**
 * Allocates memory for an asset with the given parameters.
 *
 * @param ty the type of the asset
 * @param bounding_box the bounding box containing the location and dimensions
 * of the asset when it is rendered
 * @return a pointer to the newly allocated asset
 */
static asset_t *asset_init(asset_type_t ty, SDL_Rect bounding_box) {
  asset_t *new;
  switch (ty) {
  case ASSET_IMAGE: {
    new = malloc(sizeof(image_asset_t));
    break;
  }
  case ASSET_FONT: {
    new = malloc(sizeof(text_asset_t));
    break;
  }
  case ASSET_BUTTON: {
    new = malloc(sizeof(button_asset_t));
    break;
  }
  default: {
    printf("THIS IS A BAD ERROR DELETE THIS\n");
    assert(false && "Unknown asset type");
  }
  }
  assert(new);
  new->type = ty;
  new->bounding_box = bounding_box;
  return new;
}

asset_type_t asset_get_type(asset_t *asset) { return asset->type; }

asset_t *asset_make_image(const char *filepath, SDL_Rect bounding_box) {
  image_asset_t *image_asset =
      (image_asset_t *)asset_init(ASSET_IMAGE, bounding_box);
  image_asset->texture =
      asset_cache_obj_get_or_create(image_asset->base.type, filepath);

  image_asset->body = NULL;
  return (asset_t *)image_asset;
}

// Josh Archibald
asset_t *asset_make_image_with_body(const char *filepath, body_t *body) {
  image_asset_t *new_image_asset;
  SDL_Rect *rect;

  rect = bounding_box(body);

  new_image_asset = (image_asset_t *)asset_make_image(filepath, *rect);
  free(rect);
  new_image_asset->body = body;

  return (asset_t *)new_image_asset;
}

asset_t *asset_make_back_image_with_body(const char *filepath, body_t *body) {
  image_asset_t *new_image_asset;
  SDL_Rect *rect;

  rect = bounding_box(body);

  new_image_asset = (image_asset_t *)asset_make_image(filepath, *rect);
  free(rect);
  new_image_asset->body = body;
  new_image_asset->base.type = ASSET_BACKGROUND;

  return (asset_t *)new_image_asset;
}

asset_t *asset_make_text(const char *filepath, SDL_Rect bounding_box,
                         const char *text, rgb_color_t color) {
  text_asset_t *text_asset =
      (text_asset_t *)asset_init(ASSET_FONT, bounding_box);
  text_asset->font =
      asset_cache_obj_get_or_create(text_asset->base.type, filepath);
  text_asset->color = color;
  text_asset->text = text;
  return (asset_t *)text_asset;
}

asset_t *asset_make_button(SDL_Rect bounding_box, asset_t *image_asset,
                           asset_t *text_asset, button_handler_t handler) {
  // Make the button asset
  button_asset_t *button_asset =
      (button_asset_t *)asset_init(ASSET_BUTTON, bounding_box);

  // Add an image asset if NULL was not passed
  if (image_asset != NULL) {
    assert(image_asset->type == ASSET_IMAGE);
    button_asset->image_asset = (image_asset_t *)image_asset;
  } else {
    button_asset->image_asset = NULL;
  }

  // Add a text asset if NULL was not passed
  if (text_asset != NULL) {
    assert(text_asset->type == ASSET_FONT);
    button_asset->text_asset = (text_asset_t *)text_asset;
  } else {
    button_asset->text_asset = NULL;
  }

  // Add button handler
  button_asset->handler = handler;

  // Button is not rendered by default
  button_asset->is_rendered = false;

  return (asset_t *)button_asset;
}

void asset_on_button_click(asset_t *button, state_t *state, double x,
                           double y) {
  // Make sure the button is actually a button
  assert(button->type == ASSET_BUTTON);

  // Only try to handle the asset if the button has been rendered
  button_asset_t *b = (button_asset_t *)button;
  if (b->is_rendered == true) {
    // Compute the bounds
    int xmin = button->bounding_box.x;
    int xmax = button->bounding_box.x + button->bounding_box.w;
    int ymin = button->bounding_box.y;
    int ymax = button->bounding_box.y + button->bounding_box.h;

    // If the click was within the bounds, run the handler
    if ((x > xmin) && (x < xmax) && (y > ymin) && (y < ymax)) {
      b->handler(state);
    }
  }

  // Now unrender the button
  b->is_rendered = false;
}

void asset_render(asset_t *asset) {
  // use switch case if more asset types are added
  switch (asset->type) {
  case ASSET_IMAGE: {
    image_asset_t *image = (image_asset_t *)asset;
    if (image->body != NULL) {
      SDL_Rect *rect;
      rect = bounding_box(image->body);
      sdl_render_texr_offscreen(image->texture, rect);
      free(rect);
    } else {
      sdl_render_texr_offscreen(image->texture, &(image->base.bounding_box));
    }
    break;
  }
  case ASSET_BACKGROUND: {
    image_asset_t *image = (image_asset_t *)asset;
    if (image->body != NULL) {
      SDL_Rect *rect;
      rect = bounding_box(image->body);
      sdl_render_texr_offscreen(image->texture, rect);
      free(rect);
    } else {
      sdl_render_texr_offscreen(image->texture, &(image->base.bounding_box));
    }
    break;
  }
  case ASSET_FONT: {
    text_asset_t *text_asset = (text_asset_t *)asset;
    sdl_render_text_offscreen((char *)text_asset->text, text_asset->font,
                              &(text_asset->base.bounding_box),
                              text_asset->color);
    break;
  }
  case ASSET_BUTTON: {
    button_asset_t *button_asset = (button_asset_t *)asset;

    if (button_asset->image_asset != NULL) {
      image_asset_t *image = button_asset->image_asset;
      sdl_render_texr_offscreen(image->texture,
                                &(button_asset->base.bounding_box));
    }

    if (button_asset->text_asset != NULL) {
      text_asset_t *text_asset = button_asset->text_asset;
      sdl_render_text_offscreen((char *)text_asset->text, text_asset->font,
                                &(button_asset->base.bounding_box),
                                text_asset->color);
    }

    button_asset->is_rendered = true;

    break;
  }
  }
}

void asset_scroll_render(asset_t *asset, vector_t delta_scroll) {
  if (asset->type == ASSET_IMAGE || asset->type == ASSET_BACKGROUND) {
    image_asset_t *image = (image_asset_t *)asset;
    if (image->body != NULL) {
      body_set_centroid(
          image->body,
          vec_subtract(body_get_centroid(image->body), delta_scroll));
      asset_render(asset);
    } else {
      image->base.bounding_box.x -= delta_scroll.x;
      image->base.bounding_box.y += delta_scroll.y;
      sdl_render_texr_offscreen(image->texture, &(image->base.bounding_box));
    }
  } else {
    asset_render(asset);
  }
  return;
}

void asset_rotate_render(asset_t *asset, double angle) {
  // use switch case if more asset types are added
  switch (asset->type) {
  case ASSET_IMAGE: {
    image_asset_t *image = (image_asset_t *)asset;
    if (image->body != NULL) {
      SDL_Rect *rect;
      vector_t rot_point = body_get_centroid(image->body);
      rect = bounding_box(image->body);
      sdl_render_rotate_texr(image->texture, rect, angle, rot_point);
      free(rect);
    } else {
      sdl_render_texr_offscreen(image->texture, &(image->base.bounding_box));
    }
    break;
  }
  case ASSET_FONT: {
    text_asset_t *text_asset = (text_asset_t *)asset;
    sdl_render_text_offscreen((char *)text_asset->text, text_asset->font,
                              &(text_asset->base.bounding_box),
                              text_asset->color);
    break;
  }
  case ASSET_BUTTON: {
    button_asset_t *button_asset = (button_asset_t *)asset;

    if (button_asset->image_asset != NULL) {
      image_asset_t *image = button_asset->image_asset;
      sdl_render_texr_offscreen(image->texture,
                                &(button_asset->base.bounding_box));
    }

    if (button_asset->text_asset != NULL) {
      text_asset_t *text_asset = button_asset->text_asset;
      sdl_render_text_offscreen((char *)text_asset->text, text_asset->font,
                                &(button_asset->base.bounding_box),
                                text_asset->color);
    }

    button_asset->is_rendered = true;

    break;
  }
  }
}

void asset_scroll_rotate_render(asset_t *asset, vector_t delta_scroll,
                                double angle) {
  if (asset->type == ASSET_IMAGE) {
    image_asset_t *image = (image_asset_t *)asset;
    if (image->body != NULL) {
      body_set_centroid(
          image->body,
          vec_subtract(body_get_centroid(image->body), delta_scroll));
      asset_rotate_render(asset, angle);
    } else {
      image->base.bounding_box.x -= delta_scroll.x;
      image->base.bounding_box.y += delta_scroll.y;
      sdl_render_texr_offscreen(image->texture, &(image->base.bounding_box));
    }
  } else {
    asset_render(asset);
  }
  return;
}

void asset_destroy(asset_t *asset) { free(asset); }
