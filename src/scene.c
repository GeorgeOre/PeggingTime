#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "forces.h"
#include "scene.h"
#include <stdbool.h>

const int INITIAL_BODY_SPACE = 10;

/**
 * A collection of bodies and force creators.
 * The scene automatically resizes to store arbitrarily many bodies
 * and force creators. Each force creator has a corresponding
 * auxiliary body object.
 */
struct scene {
  size_t num_bodies;
  list_t *bodies;

  // List of force creators
  size_t num_f_creators;
  list_t *f_creators;

  // List of aux bodies
  size_t num_aux_bodies;
  list_t *aux_bodies;

  // List of forcer structs that contain force creators and their bodies.
  list_t *forces;
};

// Struct made by Josh Archibald so force creators can be responsible for
// bodies.
struct force_tracker {
  // The force creator and its list of bodies.
  force_creator_t creator;
  list_t *bodies;
};

void f_creator_free(void *anything) {
  // Do nothing

  // This code is so that the pipeline passes:
  anything = anything + 1;
  anything = NULL;
}

void free_forces(struct force_tracker *force_track) {
  list_free(force_track->bodies);
  free(force_track);
}

scene_t *scene_init(void) {
  scene_t *new_scene = malloc(sizeof(scene_t));
  assert(new_scene != NULL);

  new_scene->num_bodies = 0;
  new_scene->bodies = list_init(INITIAL_BODY_SPACE, (free_func_t)body_free);

  new_scene->num_aux_bodies = 0;
  new_scene->aux_bodies = list_init(0, (free_func_t)body_aux_free);

  new_scene->num_f_creators = 0;
  new_scene->f_creators = list_init(0, (free_func_t)f_creator_free);

  new_scene->forces = list_init(0, (free_func_t)free_forces);

  return new_scene;
}

void scene_free(scene_t *scene) {
  list_free(scene->bodies);

  list_free(scene->f_creators);

  list_free(scene->aux_bodies);

  list_free(scene->forces);
  free(scene);
}

size_t scene_bodies(scene_t *scene) { return list_size(scene->bodies); }

body_t *scene_get_body(scene_t *scene, size_t index) {
  assert(index >= 0 && index < list_size(scene->bodies));
  assert(list_get(scene->bodies, index) != NULL);
  return list_get(scene->bodies, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
  list_add(scene->bodies, body);
  scene->num_bodies++;
}

void scene_remove_body(scene_t *scene, size_t index) {
  assert(index >= 0 && index < list_size(scene->bodies));
  body_remove(list_get(scene->bodies, index));
  // TODO: vvvvvvvvvvvvv  HEY IS THIS CHILL???
  // scene->num_bodies--;
}

void scene_tick(scene_t *scene, double dt) {
  struct force_tracker *curr_force_tracker;
  force_creator_t *creator;
  body_t *curr_body;
  // Execute all force creators
  force_creator_t cur_force;
  void *cur_aux;
  for (size_t i = 0; i < scene->num_f_creators; i++) {
    cur_force = list_get(scene->f_creators, i);
    cur_aux = list_get(scene->aux_bodies, i);
    cur_force(cur_aux);
  }
  // TODO: follow the pseudocode below and replace the comments once you're done
  // IMPORTANT: Remember to use `ssize_t` instead of `size_t` to prevent
  // underflow!
  for (ssize_t i = scene_bodies(scene) - 1; i >= 0; i--) {

    // printf("\tINSIDE SCENETICK: ticking body at index%zu\n", i);

    // Check if the current body is removed
    curr_body = scene_get_body(scene, i);
    // Case 1: the body is removed
    if (body_is_removed(curr_body)) {
      // Iterate through the scene's force creators

      // printf("\t\tINSIDE SCENETICK: removing body at index%zu\n", i);

      for (ssize_t j = scene->num_f_creators - 1; j >= 0; j--) {
        curr_force_tracker = list_get(scene->forces, j);
        // Iterate through the force creator's list of bodies
        for (size_t k = 0; k < list_size(curr_force_tracker->bodies); k++) {
          // If the current body is the same as the body being removed,
          // remove the current force creator and free it.
          // Remember to update the index so we don't skip any force
          // creators, and then break out of the loop.
          if (list_get(curr_force_tracker->bodies, k) == curr_body) {
            // printf("\t\tInside scenetick: removing body %zu\n", k);
            creator = list_remove(scene->f_creators, j);
            cur_aux = list_remove(scene->aux_bodies, j);
            body_aux_free(cur_aux);
            scene->num_aux_bodies--;
            f_creator_free(creator);
            scene->num_f_creators--;
            curr_force_tracker = list_remove(scene->forces, j);
            list_free(curr_force_tracker->bodies);
            free(curr_force_tracker);
            break;
          }
        }
      }
      // Remove the body from the scene and free it.
      curr_body = list_remove(scene->bodies, i);
      body_free(curr_body);
      scene->num_bodies--;
    } else {
      // Case 2: the body isn't removed
      // All we need to do is run body_tick
      body_tick(curr_body, dt);
    }
  }
}

void scene_add_force_creator(scene_t *scene, force_creator_t force_creator,
                             void *aux) {
  scene_add_bodies_force_creator(scene, force_creator, aux, list_init(0, free));
}

void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer,
                                    void *aux, list_t *bodies) {
  struct force_tracker *force_tracker_ptr;

  // Add the aux body to the scene
  list_add(scene->aux_bodies, aux);
  scene->num_aux_bodies++;

  // Add the force creator to the scene
  list_add(scene->f_creators, forcer);
  scene->num_f_creators++;

  // Create force tracker and update it with the forcer and its bodies. This
  // gets freed by the scene when scene->forces is list freed.
  force_tracker_ptr = malloc(sizeof(struct force_tracker));
  force_tracker_ptr->creator = forcer;
  force_tracker_ptr->bodies = bodies;

  // Add the force creator and list of bodies to the scene
  list_add(scene->forces, force_tracker_ptr);
}
