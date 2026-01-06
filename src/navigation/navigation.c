#include <stdio.h>
#include "navigation.h"
#include "../communication_internal/communication.h"

int web_navigate_to(web_context *ctx, char *url) {
    if (url == NULL) {
        url = "about:blank";
    }

    cJSON *response_json = NULL;
    char data[2048] = {0};
    sprintf(data, "{\"url\": \"%s\"}", url);
    DEBUG("changing url to link='%s' data='%s'", url, data);
    int resp = RCS(ctx, "/url", data, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    return resp;
}

int wait_to_page_load(web_context *ctx, int max_wait_seconds) {
    int waited = 0;
    max_wait_seconds = max_wait_seconds * 2; // .5 second intervals
    if (max_wait_seconds == 0) max_wait_seconds = INT_MAX;
    while (waited < max_wait_seconds) {
        char *current_url = NULL;
        int status = web_get_url(ctx, &current_url);
        DEBUG("current_url='%s', status=%d", current_url, status);
        if (status < 0) {
            if (current_url != NULL) free(current_url);
            continue;
        }
        if (current_url != NULL && strlen(current_url) > 0) {
            free(current_url);
            return 0; // Page loaded
        }
        free(current_url);
        web_usleep(500000); // 0.5 second
        waited++;
    }
    return -1; // Timeout
}

int web_get_url(web_context *ctx, char **url) {
    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/url", NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        if (url != NULL)
            *url = NULL;
        return resp;
    }

    if (url != NULL) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        if (!cJSON_IsString(value) || (value->valuestring == NULL)) {
            DEBUG("current URL not found in response");
            cJSON_Delete(response_json);
            web_error err = {
                .code = -1,
                .path = strdup("/url"),
                .error = strdup("no such url"),
                .message = strdup("Could not retrieve current URL from the browser."),
            };
            web_set_last_error(ctx, err);
            return -1;
        }
        *url = strdup(value->valuestring);
        DEBUG("extracted current_url='%s'", *url);
    }

    cJSON_Delete(response_json);
    return resp;
}

int web_back(web_context *ctx) {
    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/back", NULL, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    return resp;
}

int web_forward(web_context *ctx) {
    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/forward", NULL, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    return resp;
}

int web_refresh(web_context *ctx) {
    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/refresh", NULL, &response_json, WEB_POST);
    DEBUG_JSON(response_json);
    return resp;
}

int web_get_title(web_context *ctx, char **title) {
    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/title", NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    if (resp < 0) {
        if (title != NULL)
            *title = NULL;
        return resp;
    }

    if (title != NULL) {
        cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
        if (!cJSON_IsString(value) || (value->valuestring == NULL)) {
            DEBUG("page title not found in response");
            cJSON_Delete(response_json);
            web_error err = {
                .code = -1,
                .path = strdup("/title"),
                .error = strdup("no such title"),
                .message = strdup("Could not retrieve page title from the browser."),
            };
            web_set_last_error(ctx, err);
            return -1;
        }
        *title = strdup(value->valuestring);
        DEBUG("extracted title='%s'", title);
    }

    cJSON_Delete(response_json);
    return resp;
}
