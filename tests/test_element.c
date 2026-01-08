#include "test_element.h"

void test_get_element(web_context *ctx);

void test_elements(web_context *ctx) {
    INFO("\n\n\n\n------ Elements TESTS -----");

    int r;
    char *active_element;

    r = web_element_active(ctx, &active_element);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_element_active failed with code %d", r);
    }
    printf("Active Element ID: %s\n", active_element);

    r = web_get_element_shadow_root(ctx, active_element, &active_element);
    if (r < 0) {
        print_error(ctx);
        // ERROR(1, "web_get_element_shadow_root failed with code %d", r);
        WARNING("web_get_element_shadow_root failed with code %d", r);
    }
    printf("Shadow root retrieved successfully\n");
    free(active_element);

    test_get_element(ctx);



}

void test_get_element(web_context *ctx) {
    int r;
    char *active_element;

    r = web_find_element(ctx, CSS_SELECTOR, "#unique-id-btn", &active_element);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_find_element failed with code %d", r);
    }
    printf("Found Element ID: %s\n", active_element);
    free(active_element);

    r = web_find_element(ctx, XPATH_SELECTOR, "/html/body/section[4]/div/article/div/ul/li[2]/div/span[2]",
                         &active_element);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_find_element failed with code %d", r);
    }
    printf("Found Element ID: %s\n", active_element);

    char *element_text;
    r = web_get_element_text(ctx, active_element, &element_text);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_get_element_text failed with code %d", r);
    }
    printf("Element Text: %s\n", element_text);
    if (strcmp(element_text, "TARGET ELEMENT VIA XPATH") != 0) {
        WARNING("Element text does not match expected value");
    } else {
        INFO("Element text matches expected value");
    }
    free(element_text);
    free(active_element);

    r = web_find_element(ctx, LINK_TEXT_SELECTOR, "Exact Link Text", &active_element);
    if (r < 0) {
        print_error(ctx);
        WARNING("web_find_element failed with code %d", r);
    }
    printf("Found Element ID: %s\n", active_element);
    free(active_element);

    r = web_find_element(ctx, PARTIAL_LINK_TEXT_SELECTOR, "Link with Partial", &active_element);
    if (r < 0) {
        print_error(ctx);
        WARNING("web_find_element failed with code %d", r);
    }
    printf("Found Element (p txt select) ID: %s\n", active_element);
    free(active_element);

    r = web_find_element(ctx, TAG_NAME, "p", &active_element);
    if (r < 0) {
        print_error(ctx);
        WARNING("web_find_element failed with code %d", r);
    }
    printf("Found Element (tag) ID: %s\n", active_element);

    r = web_get_element_text(ctx, active_element, &element_text);
    if (r < 0) {
        print_error(ctx);
        WARNING("web_get_element_text failed with code %d", r);
    }
    printf("Element Text (tag): %s\n", element_text);
    free(element_text);
    free(active_element);

}
