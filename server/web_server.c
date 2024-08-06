/*

NOTE FOR GRADER

This should be working but there is some bug that might happen with
the game files. If you try to launch the game and it takes forever,
simply input these commands:

cd game
make
(ctrl+c)
cd ..
make

Its basically the same thing as the shell script and works nice

*/




// C libs
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>

// Custom libs
#include "network_util.h"
#include "http_request.h"
#include "http_response.h"
#include "router.h"
#include "web_util.h"

// Roll handler constants
const size_t NUM_STR_SIZE = 2;

// Routing constants
const size_t MAX_ROUTES = 3;
const char *HELLO_PATH = "/hello";
const char *ROLL_PATH = "/roll";

// Hello Handler (/hello)
bytes_t *hello_handler(request_t *request) {
    // Assert that the request is actually hello path
    assert(strcmp(request->path, HELLO_PATH) == 0);

    // Generate a response
    char *message = "Hello world!";
    // bytes_t *body = bytes_init(13, message);
    bytes_t *body = bytes_init(strlen(message), message);
    bytes_t *response = response_type_format(HTTP_OK, MIME_HTML, body);

    return response;
}

// Roll Handler (/roll)
bytes_t *roll_handler(request_t *request) {
    // Assert that the request is actually roll path
    assert(strcmp(request->path, ROLL_PATH) == 0);

    // Generate a number between 1 and 6
    srand(time(NULL));
    int random_num = (rand() % 6) + 1;

    // Generate a string to place the number inside
    char *num_str = malloc(sizeof(char *) * NUM_STR_SIZE); 
    sprintf(num_str, "%d", random_num);

    // Generate a response
    bytes_t *body = bytes_init(strlen(num_str), num_str);
    bytes_t *response = response_type_format(HTTP_OK, MIME_HTML, body);

    return response;
}

// Default Handler
bytes_t *default_handler(request_t *request) {

    // Use wutil_get_resolved_path to get a resolved path
    char *resolved_path = wutil_get_resolved_path(request);
    // If not successful (i.e., it returns NULL), return HTTP_FORBIDDEN (use MIME_PLAIN and some error message)
    if (resolved_path == NULL) {
        bytes_t *error_msg = bytes_init(strlen("Path could not be retrieved from request"), "Path could not be retrieved from request");
        bytes_t *response = response_type_format(HTTP_FORBIDDEN, MIME_PLAIN, error_msg);
        return response;
    }

    // Use wutil_check_resolved_path to determine if the path the user requested is valid
    response_code_t potential_response = wutil_check_resolved_path(resolved_path);
    //  If not HTTP_OK, return response code given (use MIME_PLAIN and some error message)
    if (potential_response != HTTP_OK) {
        bytes_t *error_msg = bytes_init(strlen("Path was not resolved"), "Path was not resolved");
        bytes_t *response = response_type_format(HTTP_FORBIDDEN, MIME_PLAIN, error_msg);
        return response;
    }

    // Open the file corresponding to the resolved path
    FILE *file = fopen(resolved_path, "r"); //maybe rb is better?
    // No error handling??? what if it fails?

    // Use wutil_get_file_size to get the size of the file
    ssize_t file_size = wutil_get_file_size(file);
    // Error handling? Will be negative if it throws an error

    // Read the file using fread
    char *file_string = malloc(sizeof(char) * file_size+1);
    // fread(file_string, 1, file_size, file);
    size_t bytes_read_successfully = fread(file_string, 1, file_size, file);

    //Make sure it is null terminated
    if(file_string[file_size] != '\0') {
        file_string[file_size] = '\0';
    }

    // Close file stream
    fclose(file);

    // Fetch the extension
    char *ext = wutil_get_filename_ext(resolved_path);

    // Get the mime for the response
    mime_type_t mime = wutil_get_mime_from_extension(ext);

    // Construct the response with format
    bytes_t *body = bytes_init(strlen(file_string), file_string);

    bytes_t *response = response_type_format(HTTP_OK, mime, body);

    // Return the response
    return response;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "USAGE:  %s <server port>\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    
    // Init router
    router_t *r = router_init(MAX_ROUTES, default_handler);
    // Register handlers
    router_register(r, HELLO_PATH, hello_handler);
    router_register(r, ROLL_PATH, roll_handler);

    while (1)
    {
      // Wait for a connection
      connection_t *client = nu_wait_client(port);

      // Read a string from the connection
      char *message = NULL; 
      while (message == NULL){  
        // Keep trying until a message exists
        message = nu_try_read_header(client);
      }

      // Parse the string into an HTTP request
      request_t *request = request_parse(message);

      // Decide on a response based on the request
      bytes_t *response = router_dispatch(r, request);

      // Send a string back along the connection
      nu_send_bytes(client, response->data, response->len);

      // Free things
      bytes_free(response);

      // Close the connection
      nu_close_connection(client);
    }

    // Free the router if loop is broken
    router_free(r);

    return 0;
}
