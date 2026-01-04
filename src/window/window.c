#include <stdio.h>
#include "window.h"

#include "communication/communication.h"

web_window_rect web_get_window_rect(web_context *ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/rect", NULL, &response_json, GET);
    DEBUG_JSON(response_json);

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");

    web_window_rect rect;
    rect.x = cJSON_GetObjectItemCaseSensitive(value, "x")->valueint;
    rect.y = cJSON_GetObjectItemCaseSensitive(value, "y")->valueint;
    rect.width = cJSON_GetObjectItemCaseSensitive(value, "width")->valueint;
    rect.height = cJSON_GetObjectItemCaseSensitive(value, "height")->valueint;

    cJSON_Delete(response_json);

    DEBUG("Got window rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    return rect;
}

int web_set_window_rect(web_context *ctx, web_window_rect rect) {
    cJSON *request_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(request_json, "x", rect.x);
    cJSON_AddNumberToObject(request_json, "y", rect.y);
    cJSON_AddNumberToObject(request_json, "width", rect.width);
    cJSON_AddNumberToObject(request_json, "height", rect.height);

    char *request_str = cJSON_PrintUnformatted(request_json);
    cJSON_Delete(request_json);

    cJSON *response_json = NULL;
    _rcs(ctx, "/window/rect", request_str, &response_json, POST);
    DEBUG_JSON(response_json);

    free(request_str);

    cJSON_Delete(response_json);

    DEBUG("Set window rect to: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    return 0;
}

web_window_rect web_maximize_window(web_context *ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/maximize", NULL, &response_json, POST);
    DEBUG_JSON(response_json);

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");

    web_window_rect rect;
    rect.x = cJSON_GetObjectItemCaseSensitive(value, "x")->valueint;
    rect.y = cJSON_GetObjectItemCaseSensitive(value, "y")->valueint;
    rect.width = cJSON_GetObjectItemCaseSensitive(value, "width")->valueint;
    rect.height = cJSON_GetObjectItemCaseSensitive(value, "height")->valueint;

    cJSON_Delete(response_json);

    DEBUG("Maximized window rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    return rect;
}

web_window_rect web_minimize_window(web_context *ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/minimize", NULL, &response_json, POST);
    DEBUG_JSON(response_json);
    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");

    web_window_rect rect;
    rect.x = cJSON_GetObjectItemCaseSensitive(value, "x")->valueint;
    rect.y = cJSON_GetObjectItemCaseSensitive(value, "y")->valueint;
    rect.width = cJSON_GetObjectItemCaseSensitive(value, "width")->valueint;
    rect.height = cJSON_GetObjectItemCaseSensitive(value, "height")->valueint;

    cJSON_Delete(response_json);

    DEBUG("Minimized window rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    return rect;
}

web_window_rect web_fullscreen_window(web_context *ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/fullscreen", NULL, &response_json, POST);
    DEBUG_JSON(response_json);
    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");

    web_window_rect rect;
    rect.x = cJSON_GetObjectItemCaseSensitive(value, "x")->valueint;
    rect.y = cJSON_GetObjectItemCaseSensitive(value, "y")->valueint;
    rect.width = cJSON_GetObjectItemCaseSensitive(value, "width")->valueint;
    rect.height = cJSON_GetObjectItemCaseSensitive(value, "height")->valueint;

    cJSON_Delete(response_json);

    DEBUG("Fullscreen window rect: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    return rect;
}

char *web_get_window(web_context *ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window", NULL, &response_json, GET);
    DEBUG_JSON(response_json);

    char *val = cJSON_GetObjectItemCaseSensitive(response_json, "value")->valuestring;
    char *handle = strdup(val);
    cJSON_Delete(response_json);
    return handle;
}

int web_close_window(web_context *ctx) {
    cJSON *response_json = NULL;
    int resp = _rcs(ctx, "/window", NULL, &response_json, DELETE);
    DEBUG_JSON(response_json);
    return resp;
}

int web_close_tab(web_context *ctx) { return web_close_window(ctx); }

int web_switch_to_window(web_context *ctx, char *handle) {
    cJSON *response_json = NULL;
    char data[1024];
    snprintf(data, sizeof(data), "{\"handle\": \"%s\"}", handle);
    int resp = _rcs(ctx, "/window", data, &response_json, POST);
    DEBUG_JSON(response_json);

    return resp;
}

int web_switch_to_tab(web_context *ctx, char *handle) {
    return web_switch_to_window(ctx, handle);
}

char **web_get_window_handles(web_context *ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/handles", NULL, &response_json, GET);
    DEBUG_JSON(response_json);

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    int count = cJSON_GetArraySize(value);
    char **handles = malloc((count + 1) * sizeof(char *));
    for (int i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(value, i);
        handles[i] = strdup(item->valuestring);
    }
    handles[count] = NULL; // Null-terminate the array
    cJSON_Delete(response_json);
    return handles;
}

char *web_new_window(web_context *ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/new", "{\"type\": \"window\"}", &response_json, POST);
    DEBUG_JSON(response_json);

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    char *val = cJSON_GetObjectItemCaseSensitive(value, "handle")->valuestring;
    char *handle = strdup(val);
    cJSON_Delete(response_json);
    return handle;
}

char *web_new_tab(web_context *ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/new", "{\"type\":\"tab\"}", &response_json, POST);
    DEBUG_JSON(response_json);

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    char *val = cJSON_GetObjectItemCaseSensitive(value, "handle")->valuestring;
    char *handle = strdup(val);
    cJSON_Delete(response_json);
    return handle;
}

int web_switch_to_page_content(web_context *ctx) {
    cJSON *response_json = NULL;
    char data[1024];
    snprintf(data, sizeof(data), "{\"id\": null}");
    int resp = _rcs(ctx, "/frame", data, &response_json, POST);
    DEBUG_JSON(response_json);
    return resp;
}

int web_switch_to_frame_index(web_context *ctx, int frame_index) {
    cJSON *response_json = NULL;
    char data[1024];
    snprintf(data, sizeof(data), "{\"id\": %d}", frame_index);
    int resp = _rcs(ctx, "/frame", data, &response_json, POST);
    DEBUG_JSON(response_json);
    return resp;
}
int web_switch_to_frame(web_context *ctx, char *frame_id) {
    if (frame_id == NULL) {
        return web_switch_to_page_content(ctx);
    }

    cJSON *response_json = NULL;
    char data[1024];
    snprintf(data, sizeof(data), "{\"id\": {\"element-6066-11e4-a52e-4f735466cecf\": \"%s\"}}", frame_id);
    int resp = _rcs(ctx, "/frame", frame_id, &response_json, POST);
    DEBUG_JSON(response_json);
    return resp;
}

int web_switch_to_frame_parent(web_context *ctx) {
    cJSON *response_json = NULL;
    int resp = _rcs(ctx, "/frame/parent", NULL, &response_json, POST);
    DEBUG_JSON(response_json);
    return resp;
}
