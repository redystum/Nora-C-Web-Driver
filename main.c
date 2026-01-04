#include <unistd.h>
#include <signal.h>

#include <stdio.h>
#include "args.h"
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

    web_context ctx = web_init(gecko, firefox, port, 1);

    web_navigate_to(&ctx, "https://www.example.com");

    sleep(2);

    web_navigate_to(&ctx, "http://localhost/simple.html");

    wait_to_page_load(&ctx, 0);

    char *current_url = web_get_url(&ctx);
    printf("Current URL: %s\n", current_url);
    free(current_url);

    INFO("\n\n\n\n------ WINDOW TESTS -----");

    char* window1 = web_get_window(&ctx);
    printf("Current Window Handle: %s\n", window1);

    char *window2 = web_new_window(&ctx);
    sleep(5);
    web_switch_to_window(&ctx, window2);
    sleep(5);

    /*
    char **handles = web_get_window_handles(&ctx);
    for (int i = 0; handles[i] != NULL; i++) {
        printf("Handle %d: %s\n", i, handles[i]);
        free(handles[i]);
    }
    free(handles);
    web_close_window(&ctx); // Close current window (window2)
    sleep(2);
    web_switch_to_window(&ctx, window1);
    char *tab1 = web_new_tab(&ctx);
    web_switch_to_tab(&ctx, tab1);
    sleep(2);
    web_navigate_to(&ctx, "https://www.example.com");
    web_close_tab(&ctx);
    sleep(2);
    web_switch_to_window(&ctx, window1);
    web_switch_to_page_content(&ctx);
    // i forgot i need to create a element first...
    web_switch_to_frame(&ctx, NULL);
    web_switch_to_frame_parent(&ctx);
    */

    INFO("\n------ OTHER TESTS -----");

    // teste error structure
    cJSON *response_json = NULL;
    RCS(&ctx, "/window/rect", "{\"width\": -5}", &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    web_error err = web_get_last_error(&ctx);
    printf("Last Error: URL=%s, Code=%d, Error=%s, Message=%s\n",
           err.url ? err.url : "NULL",
           err.code,
           err.error ? err.error : "NULL",
           err.message ? err.message : "NULL");

    web_close_window(&ctx);

    err = web_get_last_error(&ctx);
    printf("Last Error: URL=%s, Code=%d, Error=%s, Message=%s\n",
           err.url ? err.url : "NULL",
           err.code,
           err.error ? err.error : "NULL",
           err.message ? err.message : "NULL");



    /*
    char *title = web_get_title(&ctx);
    printf("Page Title: %s\n", title);
    free(title);

    INFO("back");
    web_back(&ctx);
    sleep(2);
    INFO("forward");
    web_forward(&ctx);
    sleep(2);
    INFO("refresh");
    web_refresh(&ctx);
    sleep(4);

    web_window_rect current_rect = web_get_window_rect(&ctx);
    INFO("Current rect: x=%d, y=%d, width=%d, height=%d", current_rect.x, current_rect.y, current_rect.width, current_rect.height);
    sleep(4);
    web_window_rect rect = web_fullscreen_window(&ctx);
    INFO("Fullscreen rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    sleep(4);
    rect = web_maximize_window(&ctx);
    INFO("Maximized rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    sleep(4);
    rect = web_minimize_window(&ctx);
    INFO("Minimized rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    sleep(4);

    web_maximize_window(&ctx);
    web_set_window_rect(&ctx, current_rect);

    web_timeouts timeouts = web_get_timeouts(&ctx);
    INFO("Current timeouts: script=%d ms, pageLoad=%d ms, implicit=%d ms",
         timeouts.script_ms, timeouts.page_load_ms, timeouts.implicit_wait_ms);
    sleep(2);

    timeouts.script_ms += 2000;
    timeouts.page_load_ms += 2000;
    timeouts.implicit_wait_ms += 2000;
    web_set_timeouts(&ctx, timeouts);
    timeouts = web_get_timeouts(&ctx);
    INFO("Current timeouts: script=%d ms, pageLoad=%d ms, implicit=%d ms",
         timeouts.script_ms, timeouts.page_load_ms, timeouts.implicit_wait_ms);

*/


    INFO("DONE");
    sleep(5);

    web_close(&ctx);
}
