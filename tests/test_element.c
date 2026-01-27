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

    test_get_element(ctx);

    free(active_element);
    r = web_find_element(ctx, CSS_SELECTOR, "#attr-test", &active_element);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_find_element failed with code %d", r);
    }
    char *props;
    r = web_get_element_property(ctx, active_element, "value", &props);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_get_element_property failed with code %d", r);
    }
    printf("Element Property 'value': %s\n", props);

    r = web_get_element_attribute(ctx, active_element, "data-test-id", &props);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_get_element_attribute failed with code %d", r);
    }
    printf("Element Attribute 'data-test-id': %s\n", props);

    free(active_element);
    r = web_find_element(ctx, CSS_SELECTOR, "#style-target", &active_element);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_find_element failed with code %d", r);
    }
    web_get_element_css_value(ctx, active_element, "background-color", &props);
    printf("Element CSS 'background-color': %s\n", props);
    free(props);

    r = web_get_element_tag_name(ctx, active_element, &props);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_get_element_tag_name failed with code %d", r);
    }
    printf("Element Tag Name: %s\n", props);
    free(props);

    web_rect rect;
    r = web_get_element_rect(ctx, active_element, &rect);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_get_element_rect failed with code %d", r);
    } else {
        printf("Element Rect: x=%d y=%d width=%d height=%d\n",
               rect.x, rect.y, rect.width, rect.height);
    }

    r = web_is_element_enabled(ctx, active_element, &r);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_is_element_enabled failed with code %d", r);
    }
    printf("Element is %s\n", r == 1  ? "enabled" : "disabled");

    r = web_is_element_selected(ctx, active_element, &r);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_is_element_selected failed with code %d", r);
    }
    printf("Element is %s\n", r == 1 ? "selected" : "not selected");
    free(active_element);

    r = web_find_element(ctx, CSS_SELECTOR, "#fname", &active_element);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_find_element failed with code %d", r);
    }

    web_get_computed_label(ctx, active_element, &r);
    printf("Element Computed ARIA Label: %d\n", r);
    web_get_computed_role(ctx, active_element, &r);
    printf("Element Computed ARIA Role: %d\n", r);

    r = web_send_keys_to_element(ctx, active_element, "John Doe");
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_send_keys_to_element failed with code %d", r);
    }
    r = web_get_input_value(ctx, active_element, &props);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_get_input_value failed with code %d", r);
    }
    printf("Input Value: %s\n", props);
    if (props == NULL || strcmp(props, "John Doe") != 0) {
        WARNING("Input value does not match expected value");
    } else {
        INFO("Input value matches expected value");
    }
    r = web_clear_element(ctx, active_element);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_clear_element failed with code %d", r);
    }
    free(props);
    free(active_element);

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

    char **elements_id;
    r = web_find_elements(ctx, CSS_SELECTOR, ".btn.secondary", &elements_id);
    if (r < 0) {
        print_error(ctx);
        WARNING("web_find_elements failed with code %d", r);
    } else {
        if (r == 0) {
            WARNING("No elements found with the given selector");
        } else {
            printf("Found multiple elements:\n");
            for (int i = 0; elements_id[i] != NULL; i++) {
                printf(" Element ID[%d]: %s\n", i, elements_id[i]);
                free(elements_id[i]);
            }
            free(elements_id);
        }
    }


    char *div;
    r = web_find_element(ctx, CSS_SELECTOR, "#basic-selection", &div);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_find_element failed with code %d", r);
    }

    r = web_find_element_from_element(ctx, TAG_NAME, "p", div, &active_element);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_find_element_from_element failed with code %d", r);
    }
    printf("Found Element from Element ID: %s\n", active_element);
    free(active_element);

    r = web_find_elements_from_element(ctx, TAG_NAME, "button", div, &elements_id);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_find_elements_from_element failed with code %d", r);
    } else {
        if (r == 0) {
            WARNING("No elements found with the given selector from element");
        } else {
            printf("Found multiple elements from element:\n");
            for (int i = 0; elements_id[i] != NULL; i++) {
                printf(" Element ID[%d]: %s\n", i, elements_id[i]);
                free(elements_id[i]);
            }
            free(elements_id);
        }
    }
    free(div);

    web_navigate_to(ctx, "http://localhost/");
    wait_to_page_load(ctx, 0);

    r = web_find_element(ctx, CSS_SELECTOR, "#shadow-host", &active_element);
    if (r < 0) {
        print_error(ctx);
        ERROR(1, "web_find_element failed with code %d", r);
    }

    char *shadow_root_id;
    r = web_get_element_shadow_root(ctx, active_element, &shadow_root_id);
    if (r < 0) {
        print_error(ctx);
        // ERROR(1, "web_get_element_shadow_root failed with code %d", r);
        WARNING("web_get_element_shadow_root failed with code %d", r);
    }
    printf("Shadow Root ID: %s\n", shadow_root_id);
    free(active_element);

    r = web_find_element_from_shadow_root(ctx, TAG_NAME, "p", shadow_root_id,
                                          &active_element);
    if (r < 0) {
        print_error(ctx);
        WARNING("web_find_element_from_shadow_root failed with code %d", r);
    }
    printf("Found Element from Shadow Root ID: %s\n", active_element);
    free(active_element);
}
