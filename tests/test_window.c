#include "test_window.h"

void test_window_management(web_context *ctx) {
    INFO("\n\n\n\n------ WINDOW TESTS -----");
    int r;
    char *window1;

    r = web_get_window(ctx, &window1);
    if (r < 0) {
        ERROR(1, "web_get_window failed with code %d", r);
    }
    printf("Current Window Handle: %s\n", window1);

    char *window2;
    r = web_new_window(ctx, &window2);
    if (r < 0) {
        ERROR(1, "web_new_window failed with code %d", r);
    }
    sleep(2);
    r = web_switch_to_window(ctx, window2);
    if (r < 0) {
        ERROR(1, "web_switch_to_window failed with code %d", r);
    }
    sleep(1);
    r = web_new_window(ctx, NULL);
    if (r < 0) {
        ERROR(1, "web_new_window failed with code %d", r);
    }

    char **handles;
    r = web_get_window_handles(ctx, &handles);
    if (r < 0) {
        ERROR(1, "web_get_window_handles failed with code %d", r);
    }
    for (int i = 0; handles[i] != NULL; i++) {
        printf("Handle %d: %s\n", i, handles[i]);
        free(handles[i]);
    }
    free(handles);

    r = web_close_window(ctx); // Close current window (window2)
    if (r < 0) {
        ERROR(1, "web_close_window failed with code %d", r);
    }

    sleep(2);
    r = web_switch_to_window(ctx, window1);
    if (r < 0) {
        ERROR(1, "web_switch_to_window failed with code %d", r);
    }
    char *tab1;
    r = web_new_tab(ctx, &tab1);
    if (r < 0) {
        ERROR(1, "web_new_tab failed with code %d", r);
    }
    r = web_switch_to_tab(ctx, tab1);
    if (r < 0) {
        ERROR(1, "web_switch_to_tab failed with code %d", r);
    }
    sleep(2);
    r = web_navigate_to(ctx, "https://www.example.com");
    if (r < 0) {
        ERROR(1, "web_navigate_to failed with code %d", r);
    }
    r = web_close_tab(ctx);
    if (r < 0) {
        ERROR(1, "web_close_tab failed with code %d", r);
    }
    sleep(2);
    r = web_switch_to_window(ctx, window1);
    if (r < 0) {
        ERROR(1, "web_switch_to_window failed with code %d", r);
    }
    r = web_switch_to_page_content(ctx);
    if (r < 0) {
        ERROR(1, "web_switch_to_page_content failed with code %d", r);
    }
    // i forgot i need to create a element first...
    r = web_switch_to_frame(ctx, NULL);
    if (r < 0) {
        ERROR(1, "web_switch_to_frame failed with code %d", r);
    }
    r = web_switch_to_frame_parent(ctx);
    if (r < 0) {
        ERROR(1, "web_switch_to_frame_parent failed with code %d", r);
    }

    free(window1);
    free(window2);
    free(tab1);
}

void test_window_geometry(web_context *ctx) {
    int r;
    // Geometry tests
    INFO("------ Window Geometry Tests -----");
    web_window_rect current_rect;
    r = web_get_window_rect(ctx, &current_rect);
    if (r < 0) {
        ERROR(1, "web_get_window_rect failed with code %d", r);
    }
    INFO("Current rect: x=%d, y=%d, width=%d, height=%d", current_rect.x, current_rect.y, current_rect.width,
         current_rect.height);
    sleep(4);
    web_window_rect rect;
    r = web_fullscreen_window(ctx, &rect);
    if (r < 0) {
        ERROR(1, "web_fullscreen_window failed with code %d", r);
    }
    INFO("Fullscreen rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    sleep(4);
    r = web_maximize_window(ctx, &rect);
    if (r < 0) {
        ERROR(1, "web_maximize_window failed with code %d", r);
    }
    INFO("Maximized rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    sleep(4);
    r = web_minimize_window(ctx, &rect);
    if (r < 0) {
        ERROR(1, "web_minimize_window failed with code %d", r);
    }
    INFO("Minimized rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    sleep(4);

    r = web_maximize_window(ctx, NULL);
    if (r < 0) {
        ERROR(1, "web_maximize_window failed with code %d", r);
    }
    r = web_set_window_rect(ctx, current_rect);
    if (r < 0) {
        ERROR(1, "web_set_window_rect failed with code %d", r);
    }
}

