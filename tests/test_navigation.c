#include "test_navigation.h"

void test_navigation(web_context *ctx, int run_everything) {
    int r;
    if (run_everything) {
        r = web_navigate_to(ctx, "https://www.example.com");
        if (r < 0) {
            ERROR(1, "web_navigate_to failed with code %d", r);
        }

        r = web_get_status(ctx, NULL);
        if (r < 0) {
            print_error(ctx);
            ERROR(1, "web_get_status failed with code %d", r);
        }

        sleep(2);
    }

    r = web_navigate_to(ctx, "http://localhost/simple.html");
    if (r < 0) {
        ERROR(1, "web_navigate_to failed with code %d", r);
    }

    INFO("Waiting for page to load...");
    wait_to_page_load(ctx, 0);
    INFO("Page loaded.");

    char *current_url;
    r = web_get_url(ctx, &current_url);
    if (r < 0) {
        ERROR(1, "web_get_url failed with code %d", r);
    }
    printf("Current URL: %s\n", current_url);
    free(current_url);
}

void test_navigation_history(web_context *ctx) {
    int r;
    char *title;
    r = web_get_title(ctx, &title);
    if (r < 0) {
        ERROR(1, "web_get_title failed with code %d", r);
    }
    printf("Page Title: %s\n", title);
    free(title);

    INFO("back");
    r = web_back(ctx);
    if (r < 0) {
        ERROR(1, "web_back failed with code %d", r);
    }
    sleep(2);
    INFO("forward");
    r = web_forward(ctx);
    if (r < 0) {
        ERROR(1, "web_forward failed with code %d", r);
    }
    sleep(2);
    INFO("refresh");
    r = web_refresh(ctx);
    if (r < 0) {
        ERROR(1, "web_refresh failed with code %d", r);
    }
    sleep(4);
}

