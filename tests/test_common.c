#include "test_common.h"

void print_error(web_context *ctx) {
    web_error err = web_get_last_error(ctx);
    printf("Last Error: PATH=%s, Code=%d, Error=%s, Message=%s\n",
           err.path,
           err.code,
           err.error,
           err.message);
}

