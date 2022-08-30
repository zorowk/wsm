#ifndef WSM_COMMON_H
#define WSM_COMMON_H

#include <pixman.h>
#include <wayland-client-protocol.h>

enum wsm_window_type {
    WSM_DESKTOP_WINDOW = 0,
    WSM_SWITCH_WINDOW = 1,
    WSM_LOCK_WINDOW = 2,
    WSM_SYSTEM_DIALOG_WINDOW = 3,
    WSM_STATUSBAR_WINDOW = 4,
    WSM_DROP_DOWN_PANEL_WINDOW = 5,
    WSM_SLIDE_UO_PANEL_WINDOW = 6,
    WSM_SLIDE_UO_APP_WINDOW = 7,
};

enum wsm_window_state
{
    WSM_WINDOW_CREATE = 0,
    WSM_WINDOW_SHOW_ANIMATION = 1,
    WSM_WINDOW_SHOW = 2,
    WSM_WINDOW_PINCHING = 3,
    WSM_WINDOW_HIDE_ANIMATION = 4,
    WSM_WINDOW_HIDE = 5,
};

struct wsm_window_info
{
    // Pinch
    float x_pinch_coefficient;
    float y_pinch_coefficient;

    // Window state
    enum wsm_window_state window_state;

    // Window Effect
    float window_radius;
    pixman_region32_t blur_region;

    enum wsm_window_type window_type;
};

#endif
