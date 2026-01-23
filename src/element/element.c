#include <stdio.h>
#include "element.h"

#include "communication_internal/communication.h"

int web_element_active(web_context *ctx, char **element_id) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, element_id);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/element/active", NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        return resp;
    }

    if (element_id != NULL) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        char *val = cJSON_GetObjectItemCaseSensitive(value, "element-6066-11e4-a52e-4f735466cecf")->valuestring;
        *element_id = strdup(val);
        DEBUG("Active element ID: %s", *element_id);
    }

    cJSON_Delete(response_json);
    return resp;
}

int web_get_element_shadow_root(web_context *ctx, char *element_id, char **shadow_root_id) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, element_id);
    CHECK_NULL(ctx, shadow_root_id);

    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "/element/%s/shadow", element_id);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, endpoint, NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        return resp;
    }

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    char *val = cJSON_GetObjectItemCaseSensitive(value, "shadow-6066-11e4-a52e-4f735466cecf")->valuestring;
    *shadow_root_id = strdup(val);
    DEBUG("Shadow root ID: %s", *shadow_root_id);

    cJSON_Delete(response_json);
    return resp;
}

// Common logic for finding elements
int web_find_element_logic(web_context *ctx, web_element_location_strategy strategy, char *selector,
                           char *element_id_src, char ***elements_id, int multiple, int shadow) {
    const char *using_str;
    switch (strategy) {
        case CSS_SELECTOR:
            using_str = "css selector";
            break;
        case LINK_TEXT_SELECTOR:
            using_str = "link text";
            break;
        case PARTIAL_LINK_TEXT_SELECTOR:
            using_str = "partial link text";
            break;
        case TAG_NAME:
            // Shadow roots do not support getElementsByTagName, map to css selector
            if (shadow) {
                using_str = "css selector";
            } else {
                using_str = "tag name";
            }
            break;
        case XPATH_SELECTOR:
            // XPath selectors are not supported within a Shadow Root
            if (shadow) {
                DEBUG("XPath selector is not supported in shadow roots");
                web_error err = {
                    .code = -1,
                    .message = strdup("XPath selector is not supported in shadow roots"),
                    .error = strdup("invalid argument"),
                    .path = NULL
                };
                web_set_last_error(ctx, err);
                return -1;
            }
            using_str = "xpath";
            break;
        default:
            return -1;
    }

    cJSON *request_json = cJSON_CreateObject();
    cJSON_AddStringToObject(request_json, "using", using_str);
    cJSON_AddStringToObject(request_json, "value", selector);

    char *request_str = cJSON_PrintUnformatted(request_json);
    cJSON_Delete(request_json);

    cJSON *response_json = NULL;
    int resp = 0;
    int count = 0;
    char endpoint[256];
    if (multiple) {
        if (element_id_src != NULL) {
            if (shadow) {
                snprintf(endpoint, sizeof(endpoint), "/shadow/%s/elements", element_id_src);
            } else {
                snprintf(endpoint, sizeof(endpoint), "/element/%s/elements", element_id_src);
            }
            resp = RCS(ctx, endpoint, request_str, &response_json, WEB_POST);
        } else {
            resp = RCS(ctx, "/elements", request_str, &response_json, WEB_POST);
        }

        if (resp < 0) {
            return resp;
        }

        if (elements_id != NULL) {
            cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
            char **ids = NULL;
            count = cJSON_GetArraySize(value);
            ids = malloc((count + 1) * sizeof(char *));
            for (int i = 0; i < count; i++) {
                cJSON *elem = cJSON_GetArrayItem(value, i);
                char *val = cJSON_GetObjectItemCaseSensitive(elem, "element-6066-11e4-a52e-4f735466cecf")->valuestring;
                ids[i] = strdup(val);
                DEBUG("Found element ID[%d]: %s", i, ids[i]);
            }
            ids[count] = NULL;
            *elements_id = ids;
        }
    } else {
        if (element_id_src != NULL) {
            if (shadow) {
                snprintf(endpoint, sizeof(endpoint), "/shadow/%s/element", element_id_src);
            } else {
                snprintf(endpoint, sizeof(endpoint), "/element/%s/element", element_id_src);
            }
            resp = RCS(ctx, endpoint, request_str, &response_json, WEB_POST);
        } else {
            resp = RCS(ctx, "/element", request_str, &response_json, WEB_POST);
        }

        if (resp < 0) {
            if (elements_id != NULL)
                *elements_id = NULL;
            return resp;
        }

        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        char *val = cJSON_GetObjectItemCaseSensitive(value, "element-6066-11e4-a52e-4f735466cecf")->valuestring;

        *elements_id = malloc(2 * sizeof(char *));
        (*elements_id)[0] = strdup(val);
        (*elements_id)[1] = NULL;

        DEBUG("Found element ID: %s", val);
    }
    DEBUG_JSON(response_json);
    free(request_str);

    cJSON_Delete(response_json);

    return count;
}

int web_find_element(web_context *ctx, web_element_location_strategy strategy, char *selector, char **element_id) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, selector);
    CHECK_NULL(ctx, element_id);

    char **elements_id = NULL;
    int resp = web_find_element_logic(ctx, strategy, selector, NULL, &elements_id, 0, 0);
    if (resp < 0) {
        if (element_id != NULL)
            *element_id = NULL;
        return resp;
    }

    if (element_id != NULL) {
        *element_id = elements_id[0];
    }
    free(elements_id);
    return resp;
}

int web_find_elements(web_context *ctx, web_element_location_strategy strategy, char *selector, char ***elements_id) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, selector);
    CHECK_NULL(ctx, elements_id);

    int count = web_find_element_logic(ctx, strategy, selector, NULL, elements_id, 1, 0);
    if (count < 0) {
        if (elements_id != NULL)
            *elements_id = NULL;
    }

    return count;
}

int web_find_element_from_element(web_context *ctx, web_element_location_strategy strategy, char *selector,
                                  char *element_id_src, char **element_id) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, selector);
    CHECK_NULL(ctx, element_id_src);
    CHECK_NULL(ctx, element_id);

    char **found_elements = NULL;
    int resp = web_find_element_logic(ctx, strategy, selector, element_id_src, &found_elements, 0, 0);
    if (resp < 0) {
        if (element_id != NULL)
            *element_id = NULL;
        return resp;
    }
    if (element_id != NULL) {
        *element_id = found_elements[0];
    }
    free(found_elements);
    return resp;
}

int web_find_elements_from_element(web_context *ctx, web_element_location_strategy strategy, char *selector,
                                   char *element_id_src, char ***elements_id) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, selector);
    CHECK_NULL(ctx, element_id_src);
    CHECK_NULL(ctx, elements_id);

    int count = web_find_element_logic(ctx, strategy, selector, element_id_src, elements_id, 1, 0);
    if (count < 0) {
        if (elements_id != NULL)
            *elements_id = NULL;
    }
    return count;
}

int web_find_element_from_shadow_root(web_context *ctx, web_element_location_strategy strategy, char *selector,
                                      char *shadow_root_id, char **element_id) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, selector);
    CHECK_NULL(ctx, shadow_root_id);
    CHECK_NULL(ctx, element_id);

    char **found_elements = NULL;
    int resp = web_find_element_logic(ctx, strategy, selector, shadow_root_id, &found_elements, 0, 1);
    if (resp < 0) {
        if (element_id != NULL)
            *element_id = NULL;
        return resp;
    }
    if (element_id != NULL) {
        *element_id = found_elements[0];
    }
    free(found_elements);
    return resp;
}

int web_find_elements_from_shadow_root(web_context *ctx, web_element_location_strategy strategy, char *selector,
                                       char *shadow_root_id, char ***elements_id) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, selector);
    CHECK_NULL(ctx, shadow_root_id);
    CHECK_NULL(ctx, elements_id);

    int count = web_find_element_logic(ctx, strategy, selector, shadow_root_id, elements_id, 1, 1);
    if (count < 0) {
        if (elements_id != NULL)
            *elements_id = NULL;
    }
    return count;
}


int web_get_element_text(web_context *ctx, char *element_id, char **text) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, element_id);
    CHECK_NULL(ctx, text);

    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "/element/%s/text", element_id);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, endpoint, NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        return resp;
    }

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    *text = strdup(value->valuestring);
    DEBUG("Element text: %s", *text);

    cJSON_Delete(response_json);
    return resp;
}

int web_get_prop_attr_css_logic(web_context *ctx, char *url, char **value) {
    cJSON *response_json = NULL;
    int resp = RCS(ctx, url, NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        return resp;
    }

    cJSON *val = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    if (cJSON_IsNull(val)) {
        *value = NULL;
        DEBUG("Value is null");
    } else {
        *value = strdup(val->valuestring);
        DEBUG("Value: %s", *value);
    }

    cJSON_Delete(response_json);
    return resp;

}

int web_get_element_attribute(web_context *ctx, char *element_id, char *attribute_name, char **attribute_value){
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, element_id);
    CHECK_NULL(ctx, attribute_name);
    CHECK_NULL(ctx, attribute_value);

    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "/element/%s/attribute/%s", element_id, attribute_name);
    return web_get_prop_attr_css_logic(ctx, endpoint, attribute_value);
}

int web_get_element_property(web_context *ctx, char *element_id, char *property_name, char **property_value){
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, element_id);
    CHECK_NULL(ctx, property_name);
    CHECK_NULL(ctx, property_value);

    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "/element/%s/property/%s", element_id, property_name);
    return web_get_prop_attr_css_logic(ctx, endpoint, property_value);
}

int web_get_element_css_value(web_context *ctx, char *element_id, char *css_property_name, char **css_property_value){
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, element_id);
    CHECK_NULL(ctx, css_property_name);
    CHECK_NULL(ctx, css_property_value);

    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "/element/%s/css/%s", element_id, css_property_name);
    return web_get_prop_attr_css_logic(ctx, endpoint, css_property_value);
}
