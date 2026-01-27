#include <stdio.h>
#include "window.h"

#include "communication_internal/communication.h"

int web_get_window_rect(web_context *ctx, web_rect *rect) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, rect);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window/rect", NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);

    if (resp < 0) {
        return resp;
    }

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");

    rect->x = cJSON_GetObjectItemCaseSensitive(value, "x")->valueint;
    rect->y = cJSON_GetObjectItemCaseSensitive(value, "y")->valueint;
    rect->width = cJSON_GetObjectItemCaseSensitive(value, "width")->valueint;
    rect->height = cJSON_GetObjectItemCaseSensitive(value, "height")->valueint;

    cJSON_Delete(response_json);

    DEBUG("Got window rect: x=%d, y=%d, width=%d, height=%d", rect->x, rect->y, rect->width, rect->height);
    return resp;
}

int web_set_window_rect(web_context *ctx, web_rect rect) {
    CHECK_NULL(ctx, ctx);

    cJSON *request_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(request_json, "x", rect.x);
    cJSON_AddNumberToObject(request_json, "y", rect.y);
    cJSON_AddNumberToObject(request_json, "width", rect.width);
    cJSON_AddNumberToObject(request_json, "height", rect.height);

    char *request_str = cJSON_PrintUnformatted(request_json);
    cJSON_Delete(request_json);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window/rect", request_str, &response_json, WEB_POST);
    DEBUG_JSON(response_json);

    free(request_str);

    cJSON_Delete(response_json);

    DEBUG("Set window rect to: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    return resp;
}

int web_maximize_window(web_context *ctx, web_rect *rect) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window/maximize", NULL, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        return resp;
    }

    if (rect != NULL) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        rect->x = cJSON_GetObjectItemCaseSensitive(value, "x")->valueint;
        rect->y = cJSON_GetObjectItemCaseSensitive(value, "y")->valueint;
        rect->width = cJSON_GetObjectItemCaseSensitive(value, "width")->valueint;
        rect->height = cJSON_GetObjectItemCaseSensitive(value, "height")->valueint;
        DEBUG("Maximized window rect: x=%d, y=%d, width=%d, height=%d", rect->x, rect->y, rect->width, rect->height);
    }

    cJSON_Delete(response_json);
    return resp;
}

int web_minimize_window(web_context *ctx, web_rect *rect) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window/minimize", NULL, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        return resp;
    }

    if (rect != NULL) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        rect->x = cJSON_GetObjectItemCaseSensitive(value, "x")->valueint;
        rect->y = cJSON_GetObjectItemCaseSensitive(value, "y")->valueint;
        rect->width = cJSON_GetObjectItemCaseSensitive(value, "width")->valueint;
        rect->height = cJSON_GetObjectItemCaseSensitive(value, "height")->valueint;
        DEBUG("Minimized window rect: x=%d, y=%d, width=%d, height=%d", rect->x, rect->y, rect->width, rect->height);
    }

    cJSON_Delete(response_json);
    return resp;
}

int web_fullscreen_window(web_context *ctx, web_rect *rect) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window/fullscreen", NULL, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        return resp;
    }

    if (rect != NULL) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        rect->x = cJSON_GetObjectItemCaseSensitive(value, "x")->valueint;
        rect->y = cJSON_GetObjectItemCaseSensitive(value, "y")->valueint;
        rect->width = cJSON_GetObjectItemCaseSensitive(value, "width")->valueint;
        rect->height = cJSON_GetObjectItemCaseSensitive(value, "height")->valueint;
        DEBUG("Fullscreen window rect: x=%d, y=%d, width=%d, height=%d", rect->x, rect->y, rect->width, rect->height);
    }

    cJSON_Delete(response_json);
    return resp;
}

int web_get_window(web_context *ctx, char **handle) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window", NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        if (handle != NULL)
            *handle = NULL;
        return resp;
    }

    if (handle != NULL) {
        char *val = cJSON_GetObjectItemCaseSensitive(response_json, "value")->valuestring;
        *handle = strdup(val);
    }
    cJSON_Delete(response_json);
    return resp;
}

int web_close_window(web_context *ctx) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window", NULL, &response_json, WEB_DELETE);
    DEBUG_JSON(response_json);
    return resp;
}

int web_close_tab(web_context *ctx) { return web_close_window(ctx); }

int web_switch_to_window(web_context *ctx, char *handle) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, handle);

    cJSON *response_json = NULL;
    char data[1024];
    snprintf(data, sizeof(data), "{\"handle\": \"%s\"}", handle);
    int resp = RCS(ctx, "/window", data, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    return resp;
}

int web_switch_to_tab(web_context *ctx, char *handle) {
    return web_switch_to_window(ctx, handle);
}

int web_get_window_handles(web_context *ctx, char ***handles) {
    CHECK_NULL(ctx, ctx);
    CHECK_NULL(ctx, handles);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window/handles", NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        handles = NULL;
        return resp;
    }

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    DEBUG_JSON(value)
    int count = cJSON_GetArraySize(value);
    char **handle_ = malloc((count + 1) * sizeof(char *));
    DEBUG("Number of window handles: %d", count);
    for (int i = 0; i < count; i++) {
        cJSON *item = cJSON_GetArrayItem(value, i);
        DEBUG_JSON(item)
        handle_[i] = strdup(item->valuestring);
    }
    handle_[count] = NULL;
    DEBUG("321321321321321312")
    *handles = handle_;
    DEBUG("Finished getting window handles")
    cJSON_Delete(response_json);
    return count;
}

int web_new_window(web_context *ctx, char **handle) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window/new", "{\"type\": \"window\"}", &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        if (handle != NULL)
            *handle = NULL;
        return resp;
    }

    if (handle != NULL) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        char *val = cJSON_GetObjectItemCaseSensitive(value, "handle")->valuestring;
        *handle = strdup(val);
    }
    cJSON_Delete(response_json);
    return resp;
}

int web_new_tab(web_context *ctx, char **handle) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/window/new", "{\"type\":\"tab\"}", &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        if (handle != NULL)
            *handle = NULL;
        return resp;
    }

    if (handle != NULL) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        char *val = cJSON_GetObjectItemCaseSensitive(value, "handle")->valuestring;
        *handle = strdup(val);
    }
    cJSON_Delete(response_json);
    return resp;
}

int web_switch_to_page_content(web_context *ctx) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    char data[1024];
    snprintf(data, sizeof(data), "{\"id\": null}");
    int resp = RCS(ctx, "/frame", data, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    return resp;
}

int web_switch_to_frame_index(web_context *ctx, int frame_index) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    char data[1024];
    snprintf(data, sizeof(data), "{\"id\": %d}", frame_index);
    int resp = RCS(ctx, "/frame", data, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    return resp;
}

int web_switch_to_frame(web_context *ctx, char *frame_id) {
    CHECK_NULL(ctx, ctx);

    if (frame_id == NULL) {
        return web_switch_to_page_content(ctx);
    }

    cJSON *response_json = NULL;
    char data[1024];
    snprintf(data, sizeof(data), "{\"id\": {\"element-6066-11e4-a52e-4f735466cecf\": \"%s\"}}", frame_id);
    int resp = RCS(ctx, "/frame", frame_id, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    return resp;
}

int web_switch_to_frame_parent(web_context *ctx) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/frame/parent", NULL, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    return resp;
}
