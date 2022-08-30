#define _POSIX_C_SOURCE 200112L

#include "compositor/wsm_server.h"

#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {

    char *startup_cmd = NULL;

    int c;
    while ((c = getopt(argc, argv, "s:")) != -1) {
        switch (c) {
        case 's':
            startup_cmd = optarg;
            break;
        default:
            wlr_log(WLR_ERROR, "usage: %s [-s startup-command]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }
    if (optind < argc) {
        wlr_log(WLR_ERROR, "usage: %s [-s startup-command]\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct wsm_server server = {0};
    wsm_server_autocreate(&server);

    const char *socket = wl_display_add_socket_auto(server.display);
    if (!socket) {
        wl_display_destroy(server.display);
        return EXIT_FAILURE;
    }

    if (!wlr_backend_start(server.backend)) {
        wl_display_destroy(server.display);
        return EXIT_FAILURE;
    }

    setenv("WAYLAND_DISPLAY", socket, true);
    if (startup_cmd != NULL) {
        if (fork() == 0) {
            execl("/bin/sh", "/bin/sh", "-c", startup_cmd, (void *)NULL);
        }
    }

    wlr_log(WLR_INFO, "Running Wayland compositor on WAYLAND_DISPLAY=%s",socket);
    wl_display_run(server.display);

    wl_display_destroy_clients(server.display);
    wl_display_destroy(server.display);
    return EXIT_SUCCESS;
}
