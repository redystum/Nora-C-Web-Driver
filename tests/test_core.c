#include "test_core.h"

void test_errors(web_context *ctx) {
    INFO("\n------ CORE/ERROR TESTS -----");
    int r;
    web_error err;

    // teste error structure
    cJSON *response_json = NULL;
    // RCS is defined in communication_internal/communication.h
    RCS(ctx, "/window/rect", "{\"width\": -5}", &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    err = web_get_last_error(ctx);
    printf("Last Error: PATH=%s, Code=%d, Error=%s, Message=%s\n",
           err.path ? err.path : "NULL",
           err.code,
           err.error ? err.error : "NULL",
           err.message ? err.message : "NULL");

    r = web_get_status(ctx, NULL);
    if (r < 0) {
        err = web_get_last_error(ctx);
        printf("Last Error: PATH=%s, Code=%d, Error=%s, Message=%s\n",
               err.path ? err.path : "NULL",
               err.code,
               err.error ? err.error : "NULL",
               err.message ? err.message : "NULL");
        WARNING("web_get_status failed with code %d", r);
    }

    err = web_get_last_error(ctx);
    printf("Last Error: PATH=%s, Code=%d, Error=%s, Message=%s\n",
           err.path ? err.path : "NULL",
           err.code,
           err.error ? err.error : "NULL",
           err.message ? err.message : "NULL");
}

void test_timeouts(web_context *ctx) {
    int r;
    // Timeouts
    INFO("\n------ TIMEOUTS TESTS -----");
    web_timeouts timeouts;
    r = web_get_timeouts(ctx, &timeouts);
    if (r < 0) {
        ERROR(1, "web_get_timeouts failed with code %d", r);
    }
    INFO("Current timeouts: script=%d ms, pageLoad=%d ms, implicit=%d ms",
         timeouts.script_ms, timeouts.page_load_ms, timeouts.implicit_wait_ms);
    sleep(2);

    timeouts.script_ms += 2000;
    timeouts.page_load_ms += 2000;
    timeouts.implicit_wait_ms += 2000;
    r = web_set_timeouts(ctx, timeouts);
    if (r < 0) {
        ERROR(1, "web_set_timeouts failed with code %d", r);
    }
    r = web_get_timeouts(ctx, &timeouts);
    if (r < 0) {
        ERROR(1, "web_get_timeouts failed with code %d", r);
    }
    INFO("Current timeouts: script=%d ms, pageLoad=%d ms, implicit=%d ms",
         timeouts.script_ms, timeouts.page_load_ms, timeouts.implicit_wait_ms);
}

