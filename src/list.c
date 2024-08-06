#include "list.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

const int LIST_RESIZE_FACTOR = 2; // Resizing factor when list gets full

struct list {
  void **data;
  size_t max_size;
  size_t size;
  free_func_t freer;
};

list_t *list_init(size_t initial_size, free_func_t freer) {
  // Allocate space for the list
  list_t *newlist = malloc(sizeof(list_t));
  assert(newlist != NULL);
  // Initialize vector list struct params
  newlist->max_size = initial_size;
  newlist->size = 0;

  // Malloc initial size if size is greater than 0
  assert(initial_size >= 0);
  if (initial_size == 0) {
    newlist->data = NULL;
  } else {
    newlist->data = malloc(sizeof(void *) * initial_size);
    assert(newlist->data != NULL);
  }

  newlist->freer = freer;

  return newlist;
}

void list_free(list_t *list) {
  // Free the elements in the list
  if (list->size != 0) {
    for (size_t i = 0; i < list->size; i++) {
      if (list->freer == NULL) {
        /*Do nothing if the freer does not exist*/
      } else {
        /*Free normally otherwise*/
        list->freer(list->data[i]);
      }
    }
  }
  // Free the data pointer
  free(list->data);
  // Free the list struct
  free(list);
}

size_t list_size(list_t *list) { return list->size; }

void *list_get(list_t *list, size_t index) {
  assert(index < list->size && index >= 0);
  return list->data[index];
}

void list_add(list_t *list, void *value) {
  assert(value != NULL);
  if (list->max_size == 0) {
    // If the list is 0 capped, allocate a spot and add the value
    void **resized_list_data = malloc(sizeof(void *));
    assert(resized_list_data != NULL);
    resized_list_data[0] = value;
    // Free the old data pointer
    list->data = resized_list_data;
    list->size++;
    list->max_size = 1;
  } else if (list->size == list->max_size) {
    // If the list is not empty and is at its maximum size, resize to 2x larger
    size_t old_max_size = list->max_size;
    // Allocate memory for the resized data array
    void **resized_list_data = malloc(sizeof(void *) * (old_max_size * 2));
    assert(resized_list_data != NULL);
    for (size_t i = 0; i < old_max_size; i++) {
      resized_list_data[i] = list->data[i];
    }
    resized_list_data[old_max_size] = value;
    // Free the old data pointer
    free(list->data);
    list->data = resized_list_data;
    list->size++;
    list->max_size = old_max_size * 2;
  } else {
    // If the list is not full, just add the value
    list->data[list->size] = value;
    list->size++;
  }
}

void *list_remove(list_t *list, size_t index) {
  assert(index >= 0 && index < list->size);
  void *removed_element = list->data[index];
  // Shift elements if index is not the end of the list
  if (index != list->size - 1) {
    removed_element = list->data[index];
    for (size_t i = index; i < list->size - 1; i++) {
      list->data[i] = list->data[i + 1];
    }
  }
  list->size--;
  return removed_element;
}
