#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "strarray.h"

strarray_t *strarray_init(size_t len) {
    strarray_t *mystrarray = malloc(sizeof(strarray_t));
    mystrarray->length = len;

    char **words = malloc(sizeof(char *) * (len));
    for (size_t i = 0; i < len; i++) {
        words[i] = NULL;
    }
    mystrarray->data = words;
    return mystrarray;
}

void strarray_free(strarray_t *arr) {
    for (size_t i = 0; i < arr->length; i++) {
        free(arr->data[i]);
    }
    free(arr->data);
    free(arr);
}
