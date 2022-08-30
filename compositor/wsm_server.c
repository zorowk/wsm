#define _POSIX_C_SOURCE 200112L
#include "wsm_server.h"

#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <wayland-util.h>
#include <wayland-server-core.h>

#include <wlr/types/wlr_surface.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/log.h>

static void output_handle_frame(struct wl_listener *listener, void *data) {
    struct wsm_output *output = wl_container_of(listener, output, frame);

    if (!wlr_scene_output_commit(output->scene_output)) {
        return;
    }

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    struct wsm_surface *surface;
    wl_list_for_each(surface, &output->server->surfaces, link) {
        wlr_surface_send_frame_done(surface->surface, &now);
    }
}

static void server_handle_new_output(struct wl_listener *listener, void *data) {
    struct wsm_server *server = wl_container_of(listener, server, new_output);
    struct wlr_output *wlr_output = data;

    wlr_output_init_render(wlr_output, server->allocator, server->renderer);

    struct wsm_output *output =
        calloc(1, sizeof(struct wsm_output));
    output->output = wlr_output;
    output->server = server;
    output->frame.notify = output_handle_frame;
    wl_signal_add(&wlr_output->events.frame, &output->frame);
    wl_list_insert(&server->outputs, &output->link);

    output->scene_output = wlr_scene_output_create(server->scene, wlr_output);

    if (!wl_list_empty(&wlr_output->modes)) {
        struct wlr_output_mode *mode = wlr_output_preferred_mode(wlr_output);
        wlr_output_set_mode(wlr_output, mode);
        wlr_output_commit(wlr_output);
    }

    wlr_output_create_global(wlr_output);
}

static void surface_handle_commit(struct wl_listener *listener, void *data) {
    struct wsm_surface *surface = wl_container_of(listener, surface, commit);
    wlr_scene_rect_set_size(surface->border,
            surface->surface->current.width,
            surface->surface->current.height);
}

static void surface_handle_destroy(struct wl_listener *listener, void *data) {
    struct wsm_surface *surface = wl_container_of(listener, surface, destroy);
    wlr_scene_node_destroy(&surface->scene_surface->node);
    wlr_scene_node_destroy(&surface->border->node);
    wl_list_remove(&surface->destroy.link);
    wl_list_remove(&surface->link);
    free(surface);
}

static void server_handle_new_surface(struct wl_listener *listener,
        void *data) {
    struct wsm_server *server = wl_container_of(listener, server, new_surface);
    struct wlr_surface *wlr_surface = data;

    int pos = 50 * wl_list_length(&server->surfaces);

    struct wsm_surface *surface = calloc(1, sizeof(struct wsm_surface));
    surface->surface = wlr_surface;
    surface->commit.notify = surface_handle_commit;
    wl_signal_add(&wlr_surface->events.commit, &surface->commit);
    surface->destroy.notify = surface_handle_destroy;
    wl_signal_add(&wlr_surface->events.destroy, &surface->destroy);

    /* Border dimensions will be set in surface.commit handler */
    surface->border = wlr_scene_rect_create(&server->scene->node,
            0, 0, (float[4]){ 0.5f, 0.5f, 0.5f, 1 });
    wlr_scene_node_set_position(&surface->border->node, pos, pos);

    surface->scene_surface =
        wlr_scene_surface_create(&server->scene->node, wlr_surface);
    wl_list_insert(server->surfaces.prev, &surface->link);

    wlr_scene_node_set_position(&surface->scene_surface->node, pos, pos);
}

void wsm_server_autocreate(struct wsm_server *server)
{
    server->surface_offset = 0;
    server->display = wl_display_create();
    server->backend = wlr_backend_autocreate(server->display);
    server->scene = wlr_scene_create();
    server->renderer = wlr_renderer_autocreate(server->backend);
    wlr_renderer_init_wl_display(server->renderer, server->display);
    server->allocator = wlr_allocator_autocreate(server->backend, server->renderer);
    server->compositor = wlr_compositor_create(server->display, server->renderer);
    wlr_xdg_shell_create(server->display);

    wl_list_init(&server->outputs);
    wl_list_init(&server->surfaces);

    server->new_output.notify = server_handle_new_output;
    wl_signal_add(&server->backend->events.new_output, &server->new_output);

    server->new_surface.notify = server_handle_new_surface;
    wl_signal_add(&server->compositor->events.new_surface, &server->new_surface);
}
