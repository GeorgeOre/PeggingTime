#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "http_request.h"
#include "ll.h"
#include "mystr.h"

request_t *request_init(const char *method, const char *path , const char *http_version) {
    // Make request instance
    request_t *request = malloc(sizeof(request_t));

    // Copy all params into request instance
    char *methodcpy = malloc(sizeof(char) * strlen(method)+1);
    strcpy(methodcpy, method);
    request->method = methodcpy;
    
    char *pathcpy = malloc(sizeof(char) * strlen(path)+1);
    strcpy(pathcpy, path);
    request->path = pathcpy;

    char *versioncpy = malloc(sizeof(char) * strlen(http_version)+1);
    strcpy(versioncpy, http_version);
    request->http_version = versioncpy;
    
    // Initalize headers linked list
    request->headers = ll_init();
    
    return request;
}

request_t *request_parse(const char *contents) { 
    // Fetch first line
    size_t end_line_one = mystr_indexof(contents, '\r', 0);

    // Make a copy
    char *first_line = malloc(sizeof(char) * end_line_one+1);    
    strncpy(first_line, contents, end_line_one);
    first_line[end_line_one] = '\0';
    
    // Separate first line into the method, path, and version
    strarray_t *line_one_parts = mystr_split(first_line, ' ');
    char *method = line_one_parts->data[0];
    char *path = line_one_parts->data[1];
    char *version = line_one_parts->data[2];
    
    // Create the request instance
    request_t *request = request_init(method, path, version);

    // Parse all of the lines for data pairs
    strarray_t *lines = mystr_split(contents, '\n');
    
    // Declare local vars
    char *templine;
    size_t keyend;
    size_t valueend;
    size_t keystart = 0;
    size_t valuestart;
    size_t keylen;
    size_t valuelen;

    // Skip line 0 and fetch data pairs
    for (size_t i = 1; i < lines->length-1; i++) {
      // For every line
      templine = lines->data[i];
      
      // Parse string copy into key and value
      keyend = mystr_indexof(templine, ':', 0);
      valueend = mystr_indexof(templine, '\r', keyend+2);
      valuestart = keyend + 2;
      keylen = keyend - keystart;
      valuelen = valueend - valuestart;
      
      // Allocate space for key and value pair
      char *key = malloc(sizeof(char) * keylen+1);
      key[keylen] = '\0';
      char *value = malloc(sizeof(char) * valuelen+1);
      value[valuelen] = '\0';

      // Place data into key and value
      for (size_t j = 0; j < keyend; j++)
      {
        key[j] = templine[j];
      }
      
      for (size_t j = valuestart; j < valueend; j++)
      {
        value[j-valuestart] = templine[j];
      }

      // Place pair into the linked list
      ll_put(request->headers, key, value);
    }

    // Free intermediate structures
    free(first_line);
    strarray_free(line_one_parts);
    strarray_free(lines);

    return request;
}

void request_free(request_t *req) {
    free(req->method);
    free(req->path);
    free(req->http_version);
    ll_free(req->headers);
    free(req);
}
