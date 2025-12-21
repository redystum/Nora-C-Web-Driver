#include <stdio.h>
#include "navigation.h"
#include "../communication/communication.h"

/**
 * \brief Change current URL
 * \param ctx web context
 * \param link new link
 * \return 0=ok
 */
int web_navigate_to(web_context ctx, char *url) {
    cJSON *response_json = NULL;
    char data[2048] = {0};
    sprintf(data, "{\"url\": \"%s\"}", url);
    DEBUG("changing url to link='%s' data='%s'", url ? url : "(null)",
          data ? data : "(null)");
    _rcs(ctx, "/url", data, &response_json, POST);
    DEBUG("response (truncated): %.200s", response_json);
    return 0;
}

/**
 * \brief Wait until page is loaded
 * \param ctx web context
 * \param max_wait_seconds 0=unlimited
 * \return 0=ok, -1=timeout
 */
int wait_to_page_load(web_context ctx, int max_wait_seconds) {
    int waited = 0;
    max_wait_seconds = max_wait_seconds * 2; // .5 second intervals
    if (max_wait_seconds == 0) max_wait_seconds = INT_MAX;
    while (waited < max_wait_seconds) {
        char *current_url = web_get_url(ctx);
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

/**
 * \brief Get current URL
 * \param ctx web context
 * \return current URL (as a pointer)
 */
char *web_get_url(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/url", NULL, &response_json, GET);
    DEBUG("response (truncated): %.200s", response_json);

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    if (!cJSON_IsString(value) || (value->valuestring == NULL)) {
        DEBUG("current URL not found in response");
        cJSON_Delete(response_json);
        return NULL;
    }
    char *current_url = strdup(value->valuestring);
    cJSON_Delete(response_json);

    DEBUG("extracted current_url='%s'", current_url);
    return current_url;
}

/**
 * \brief Navigate back in browser history
 * \param ctx web context
 * \return 0=ok
 */
int web_back(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/back", NULL, &response_json, POST);
    DEBUG("response (truncated): %.200s", response_json);
    return 0;
}

/**
 * \brief Navigate forward in browser history
 * \param ctx web context
 * \return 0=ok
 */
int web_forward(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/forward", NULL, &response_json, POST);
    DEBUG("response (truncated): %.200s", response_json);
    return 0;
}

/**
 * \brief Refresh current page
 * \param ctx web context
 * \return 0=ok
 */
int web_refresh(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/refresh", NULL, &response_json, POST);
    DEBUG("response (truncated): %.200s", response_json);
    return 0;
}

/**
 * \brief Get current page title
 * \param ctx web context
 * \return current page title (as a pointer)
 */
char *web_get_title(web_context ctx) {
    cJSON *response_json = NULL;
    _rcs(ctx, "/title", NULL, &response_json, GET);
    DEBUG("response (truncated): %.200s", response_json);

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    if (!cJSON_IsString(value) || (value->valuestring == NULL)) {
        DEBUG("page title not found in response");
        cJSON_Delete(response_json);
        return NULL;
    }
    char *title = strdup(value->valuestring);
    cJSON_Delete(response_json);

    DEBUG("extracted title='%s'", title);
    return title;
}
