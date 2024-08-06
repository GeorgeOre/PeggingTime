#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "router.h"

typedef struct route {
    char *path;
    route_handler_t handler;
} route_t;

struct router {
    route_handler_t fallback;
    size_t max_routes;
    size_t num_routes;
    route_t routes[];
};

router_t *router_init(size_t max_routes, route_handler_t fallback) {
    router_t *router = malloc(sizeof(router_t) + max_routes*sizeof(route_t));
    assert(router!=NULL);
    router->fallback = fallback;
    router->max_routes = max_routes;
    router->num_routes = 0;
    for (size_t i = 0; i < max_routes; i++) {
        router->routes[i].handler = NULL;
    }    
    return router;
}

void router_register(router_t *router, const char *path, route_handler_t handler) {
// Numerical vars
    size_t max_routes = router->max_routes;
    size_t num_routes = router->num_routes;

    // Check if max number of routes have already been registered
    if (num_routes < max_routes){
        // If there is space, resgister the route
        router->routes[num_routes].handler = handler;
        router->routes[num_routes].path = (char *)path;
        router->num_routes++;
    } else {
        // If max routes have been set, search for a replacement route
        for (size_t i = 0; i < max_routes; i++) {
            // Make sure that the string values are valid
            if (router->routes[i].path != NULL && path != NULL) {
                char *test_path = router->routes[i].path;
                if (strcmp(test_path, path) == 0) {
                    // If a path match was found, replace the handler
                    router->routes[i].handler = handler;
                }
            }
        }
        // If no replacement was found, nothing will occur
    }
}

bytes_t *router_dispatch(router_t *router, request_t *request) {
    // Return var
    bytes_t *data;

    // Search every registered route for a path match
    size_t num_routes = router->num_routes;
    for (size_t i = 0; i < num_routes; i++) {
        char *r_path = router->routes[i].path;
        // Make sure the values are valid
        if (r_path != NULL && request->path != NULL) {
            if (strcmp(r_path, request->path) == 0) {
                // Fetch data, free request and return data if matched
                data = (bytes_t *)router->routes[i].handler(request);
                request_free(request);
                return data;
            }
        }
    }
    // If the routing failed, use fallback to genereate data and then free request
    data = router->fallback(request);
    request_free(request);
    return data;
}

void router_free(router_t *router) {
    free(router);
}
