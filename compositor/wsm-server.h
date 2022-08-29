#ifndef WSM_SERVER_H
#define WSM_SERVER_H

#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_surface.h>
#include <wlr/util/log.h>

struct wsm_server {
    struct wl_display *display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;
    struct wlr_scene *scene;
    struct wlr_compositor *compositor;

    uint32_t surface_offset;

    struct wl_list outputs;
    struct wl_list surfaces;

    struct wl_listener new_output;
    struct wl_listener new_surface;
};

struct wsm_surface {
    struct wlr_surface *surface;
    struct wlr_scene_surface *scene_surface;
    struct wlr_scene_rect *border;
    struct wl_list link;

    struct wl_listener commit;
    struct wl_listener destroy;
};

struct wsm_output {
    struct wl_list link;
    struct wsm_server *server;
    struct wlr_output *output;
    struct wlr_scene_output *scene_output;

    struct wl_listener frame;
};

void wsm_server_autocreate(struct wsm_server *server);

#endif
