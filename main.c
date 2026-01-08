#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#include <stdio.h>
#include "args.h"
#include "include/web_driver.h"
#include "tests/test_common.h"
#include "tests/test_element.h"
#include "tests/test_window.h"
#include "tests/test_navigation.h"
#include "tests/test_core.h"



int main(int argc, char *argv[]) {
    struct gengetopt_args_info args;

    bool RUN_EVERYTHING = false;

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
        print_error(&ctx);
        ERROR(1, "web_init failed with code %d", r);
    }

    test_navigation(&ctx, RUN_EVERYTHING);

    test_elements(&ctx);

    if (!RUN_EVERYTHING) {
        INFO("DONE");
        sleep(2);
        r = web_close(&ctx);
        if (r < 0) {
            ERROR(1, "web_close failed with code %d", r);
        }
        cmdline_parser_free(&args);
        return 0;
    }

    test_window_management(&ctx);

    test_errors(&ctx);

    test_navigation_history(&ctx);

    test_window_geometry(&ctx);

    test_timeouts(&ctx);

    INFO("DONE");
    sleep(5);

    r = web_close(&ctx);
    if (r < 0) {
        ERROR(1, "web_close failed with code %d", r);
    }

    cmdline_parser_free(&args);
    return 0;
}
