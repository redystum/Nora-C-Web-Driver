#include <unistd.h>
#include <signal.h>

#include <stdio.h>
#include "args.h"
#include "communication_internal/communication.h" // just for testing, this should not be used
#include "include/web_driver.h"

int main(int argc, char *argv[]) {
    struct gengetopt_args_info args;

    if (cmdline_parser(argc, argv, &args) != 0) {
        ERROR(1, "Error parsing command line");
        return 1;
    }

    char *gecko = args.gecko_arg;
    char *firefox = args.firefox_arg;
    int port = args.port_arg;

    printf("------\n");
    printf("%s\n", gecko);
    printf("%s\n", firefox);
    printf("%i\n", port);
    printf("------\n");

    web_context ctx;
    int r = web_init(&ctx, gecko, firefox, port, 1);
    if (r < 0) {
        web_error web_error = web_get_last_error(&ctx);
        printf("Last Error: PATH=%s, Code=%d, Error=%s, Message=%s\n",
               web_error.path,
               web_error.code,
               web_error.error,
               web_error.message);
        ERROR(1, "web_init failed with code %d", r);
    }

    r = web_navigate_to(&ctx, "https://www.example.com");
    if (r < 0) {
        ERROR(1, "web_navigate_to failed with code %d", r);
    }

    r = web_get_status(&ctx, NULL);
    if (r < 0) {
        web_error err = web_get_last_error(&ctx);
        printf("Last Error: PATH=%s, Code=%d, Error=%s, Message=%s\n",
               err.path ? err.path : "NULL",
               err.code,
               err.error ? err.error : "NULL",
               err.message ? err.message : "NULL");
        ERROR(1, "web_close_window failed with code %d", r);
    }

    sleep(2);

    r = web_navigate_to(&ctx, "http://localhost/simple.html");
    if (r < 0) {
        ERROR(1, "web_navigate_to failed with code %d", r);
    }

    INFO("Waiting for page to load...");
    wait_to_page_load(&ctx, 0);
    INFO("Page loaded.");

    char *current_url;
    r = web_get_url(&ctx, &current_url);
    if (r < 0) {
        ERROR(1, "web_get_url failed with code %d", r);
    }
    printf("Current URL: %s\n", current_url);
    free(current_url);

    INFO("\n\n\n\n------ WINDOW TESTS -----");

    char* window1;
    r = web_get_window(&ctx, &window1);
    if (r < 0) {
        ERROR(1, "web_get_window failed with code %d", r);
    }
    printf("Current Window Handle: %s\n", window1);

    char *window2;
    r = web_new_window(&ctx, &window2);
    if (r < 0) {
        ERROR(1, "web_new_window failed with code %d", r);
    }
    sleep(2);
    r = web_switch_to_window(&ctx, window2);
    if (r < 0) {
        ERROR(1, "web_switch_to_window failed with code %d", r);
    }
    sleep(1);
    r = web_new_window(&ctx, NULL);
    if (r < 0) {
        ERROR(1, "web_new_window failed with code %d", r);
    }

    char **handles;
    r = web_get_window_handles(&ctx, &handles);
    if (r < 0) {
        ERROR(1, "web_get_window_handles failed with code %d", r);
    }
    for (int i = 0; handles[i] != NULL; i++) {
        printf("Handle %d: %s\n", i, handles[i]);
        free(handles[i]);
    }
    free(handles);

    r = web_close_window(&ctx); // Close current window (window2)
    if (r < 0) {
        ERROR(1, "web_close_window failed with code %d", r);
    }

    sleep(2);
    r = web_switch_to_window(&ctx, window1);
    if (r < 0) {
        ERROR(1, "web_switch_to_window failed with code %d", r);
    }
    char *tab1;
    r = web_new_tab(&ctx, &tab1);
    if (r < 0) {
        ERROR(1, "web_new_tab failed with code %d", r);
    }
    r = web_switch_to_tab(&ctx, tab1);
    if (r < 0) {
        ERROR(1, "web_switch_to_tab failed with code %d", r);
    }
    sleep(2);
    r = web_navigate_to(&ctx, "https://www.example.com");
    if (r < 0) {
        ERROR(1, "web_navigate_to failed with code %d", r);
    }
    r = web_close_tab(&ctx);
    if (r < 0) {
        ERROR(1, "web_close_tab failed with code %d", r);
    }
    sleep(2);
    r = web_switch_to_window(&ctx, window1);
    if (r < 0) {
        ERROR(1, "web_switch_to_window failed with code %d", r);
    }
    r = web_switch_to_page_content(&ctx);
    if (r < 0) {
        ERROR(1, "web_switch_to_page_content failed with code %d", r);
    }
    // i forgot i need to create a element first...
    r = web_switch_to_frame(&ctx, NULL);
    if (r < 0) {
        ERROR(1, "web_switch_to_frame failed with code %d", r);
    }
    r = web_switch_to_frame_parent(&ctx);
    if (r < 0) {
        ERROR(1, "web_switch_to_frame_parent failed with code %d", r);
    }

    INFO("\n------ OTHER TESTS -----");

    // teste error structure
    cJSON *response_json = NULL;
    RCS(&ctx, "/window/rect", "{\"width\": -5}", &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    web_error err = web_get_last_error(&ctx);
    printf("Last Error: PATH=%s, Code=%d, Error=%s, Message=%s\n",
           err.path ? err.path : "NULL",
           err.code,
           err.error ? err.error : "NULL",
           err.message ? err.message : "NULL");

    r = web_get_status(&ctx, NULL);
    if (r < 0) {
        err = web_get_last_error(&ctx);
        printf("Last Error: PATH=%s, Code=%d, Error=%s, Message=%s\n",
               err.path ? err.path : "NULL",
               err.code,
               err.error ? err.error : "NULL",
               err.message ? err.message : "NULL");
        ERROR(1, "web_close_window failed with code %d", r);
    }

    err = web_get_last_error(&ctx);
    printf("Last Error: PATH=%s, Code=%d, Error=%s, Message=%s\n",
           err.path ? err.path : "NULL",
           err.code,
           err.error ? err.error : "NULL",
           err.message ? err.message : "NULL");


    r = web_switch_to_window(&ctx, window1);
    if (r < 0) {
        ERROR(1, "web_switch_to_window failed with code %d", r);
    }
    char *title;
    r = web_get_title(&ctx, &title);
    if (r < 0) {
        ERROR(1, "web_get_title failed with code %d", r);
    }
    printf("Page Title: %s\n", title);
    free(title);

    INFO("back");
    r = web_back(&ctx);
    if (r < 0) {
        ERROR(1, "web_back failed with code %d", r);
    }
    sleep(2);
    INFO("forward");
    r = web_forward(&ctx);
    if (r < 0) {
        ERROR(1, "web_forward failed with code %d", r);
    }
    sleep(2);
    INFO("refresh");
    r = web_refresh(&ctx);
    if (r < 0) {
        ERROR(1, "web_refresh failed with code %d", r);
    }
    sleep(4);

    web_window_rect current_rect;
    r = web_get_window_rect(&ctx, &current_rect);
    if (r < 0) {
        ERROR(1, "web_get_window_rect failed with code %d", r);
    }
    INFO("Current rect: x=%d, y=%d, width=%d, height=%d", current_rect.x, current_rect.y, current_rect.width, current_rect.height);
    sleep(4);
    web_window_rect rect;
    r = web_fullscreen_window(&ctx, &rect);
    if (r < 0) {
        ERROR(1, "web_fullscreen_window failed with code %d", r);
    }
    INFO("Fullscreen rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    sleep(4);
    r = web_maximize_window(&ctx, &rect);
    if (r < 0) {
        ERROR(1, "web_maximize_window failed with code %d", r);
    }
    INFO("Maximized rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    sleep(4);
    r = web_minimize_window(&ctx, &rect);
    if (r < 0) {
        ERROR(1, "web_minimize_window failed with code %d", r);
    }
    INFO("Minimized rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    sleep(4);

    r = web_maximize_window(&ctx, NULL);
    if (r < 0) {
        ERROR(1, "web_maximize_window failed with code %d", r);
    }
    r = web_set_window_rect(&ctx, current_rect);
    if (r < 0) {
        ERROR(1, "web_set_window_rect failed with code %d", r);
    }

    web_timeouts timeouts;
    r = web_get_timeouts(&ctx, &timeouts);
    if (r < 0) {
        ERROR(1, "web_get_timeouts failed with code %d", r);
    }
    INFO("Current timeouts: script=%d ms, pageLoad=%d ms, implicit=%d ms",
         timeouts.script_ms, timeouts.page_load_ms, timeouts.implicit_wait_ms);
    sleep(2);

    timeouts.script_ms += 2000;
    timeouts.page_load_ms += 2000;
    timeouts.implicit_wait_ms += 2000;
    r = web_set_timeouts(&ctx, timeouts);
    if (r < 0) {
        ERROR(1, "web_set_timeouts failed with code %d", r);
    }
    r = web_get_timeouts(&ctx, &timeouts);
    if (r < 0) {
        ERROR(1, "web_get_timeouts failed with code %d", r);
    }
    INFO("Current timeouts: script=%d ms, pageLoad=%d ms, implicit=%d ms",
         timeouts.script_ms, timeouts.page_load_ms, timeouts.implicit_wait_ms);

    INFO("DONE");
    sleep(5);

    r = web_close(&ctx);
    if (r < 0) {
        ERROR(1, "web_close failed with code %d", r);
    }

    free(window1);
    free(window2);
    free(tab1);
    cmdline_parser_free(&args);
    return 0;
}
