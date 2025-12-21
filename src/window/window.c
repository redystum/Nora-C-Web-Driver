#include <stdio.h>
#include "window.h"

#include "communication/communication.h"

web_window_rect web_get_window_rect(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/rect", NULL, &response_json, GET);
    _debug_response(response_json);

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

int web_set_window_rect(web_context ctx, web_window_rect rect) {
    cJSON *request_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(request_json, "x", rect.x);
    cJSON_AddNumberToObject(request_json, "y", rect.y);
    cJSON_AddNumberToObject(request_json, "width", rect.width);
    cJSON_AddNumberToObject(request_json, "height", rect.height);

    char *request_str = cJSON_PrintUnformatted(request_json);
    cJSON_Delete(request_json);

    cJSON *response_json = NULL;
    _rcs(ctx, "/window/rect", request_str, &response_json, POST);
    _debug_response(response_json);

    free(request_str);

    cJSON_Delete(response_json);

    DEBUG("Set window rect to: x=%d, y=%d, width=%d, height=%d", rect.x, rect.y, rect.width, rect.height);
    return 0;
}

web_window_rect web_maximize_window(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/maximize", NULL, &response_json, POST);
    _debug_response(response_json);

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

web_window_rect web_minimize_window(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/minimize", NULL, &response_json, POST);
    _debug_response(response_json);
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

web_window_rect web_fullscreen_window(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/window/fullscreen", NULL, &response_json, POST);
    _debug_response(response_json);
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
