#include <stdio.h>
#include "test_document.h"

void test_document(web_context *ctx) {
    INFO("\n\n\n\n------ Document TESTS -----");

    int r = 0;

    // web_navigate_to(ctx, "https://www.example.com");

    char *source = NULL;
    r = web_get_page_source(ctx, &source);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_get_page_source failed with code %d", r);
    }
    printf("Page Source:\n%s\n", source);
    free(source);

}
