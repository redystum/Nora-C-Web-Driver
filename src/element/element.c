#include <stdio.h>
#include "element.h"

#include "communication_internal/communication.h"

int web_element_active(web_context *ctx, char **element_id) {
    if (element_id == NULL) return -1;

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
    if (element_id == NULL || shadow_root_id == NULL) {
        return -1;
    }

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

int web_find_element(web_context *ctx, web_element_location_strategy strategy, char *selector, char **element_id) {
    if (ctx == NULL || selector == NULL) {
        return -1;
    }

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
            using_str = "tag name";
            break;
        case XPATH_SELECTOR:
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
    int resp = RCS(ctx, "/element", request_str, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    free(request_str);

    if (resp < 0) {
        if (element_id != NULL)
        *element_id = NULL;
        return resp;
    }

    if (element_id != NULL) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        char *val = cJSON_GetObjectItemCaseSensitive(value, "element-6066-11e4-a52e-4f735466cecf")->valuestring;
        *element_id = strdup(val);
        DEBUG("Found element ID: %s", *element_id);
    }

    cJSON_Delete(response_json);
    return resp;
}




int web_get_element_text(web_context *ctx, char *element_id, char **text) {
    if (ctx == NULL || element_id == NULL || text == NULL) {
        return -1;
    }

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
