#include <stdio.h>
#include "test_document.h"

void test_document(web_context *ctx) {
    INFO("\n\n\n\n------ Document TESTS -----");

    int r = 0;

    char *source = NULL;
    r = web_get_page_source(ctx, &source);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_get_page_source failed with code %d", r);
    }
    printf("Page Source:\n%s\n", source);
    free(source);

    cJSON *script_result = NULL;
    r = web_execute_script_sync(ctx, "return document.title;", NULL, &script_result);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_execute_script_sync failed with code %d", r);
    }

    if (cJSON_IsString(script_result)) {
        printf("Document Title: %s\n", cJSON_PrintUnformatted(script_result));
    } else {
        ERROR(1, "Unexpected script result format");
    }
    cJSON_Delete(script_result);

    cJSON *args = cJSON_CreateArray();
    cJSON_AddItemToArray(args, cJSON_CreateNumber(5));
    cJSON_AddItemToArray(args, cJSON_CreateNumber(10));
    r = web_execute_script_sync(ctx, "return arguments[0] + arguments[1];", args, &script_result);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_execute_script_sync with args failed with code %d", r);
    }
    if (cJSON_IsNumber(script_result)) {
        printf("Script Result (5 + 10): %f\n", script_result->valuedouble);
    } else {
        ERROR(1, "Unexpected script result format for sum");
    }

}
