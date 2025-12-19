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

    web_navigate_to(ctx, "https://www.example.com");

    sleep(2);

    web_navigate_to(ctx, "http://localhost/simple.html");

    wait_to_page_load(ctx, 0);

    char *current_url = web_get_url(ctx);
    printf("Current URL: %s\n", current_url);

    free(current_url);

    char *title = web_get_title(ctx);
    printf("Page Title: %s\n", title);
    free(title);

    INFO("back");
    web_back(ctx);
    sleep(2);
    INFO("forward");
    web_forward(ctx);
    sleep(2);
    INFO("refresh");
    web_refresh(ctx);

    INFO("DONE");
    sleep(5);

    web_close(&ctx);
}
