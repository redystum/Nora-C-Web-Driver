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
    char response[2048] = {0};
    char data[2048];
    sprintf(data, "{\"url\": \"%s\"}", url);
    DEBUG("changing url to link='%s' data='%s'", url ? url : "(null)",
          data ? data : "(null)");
    _rcs(ctx, "/url", data, response);
    DEBUG("response (truncated): %.200s", response);
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

char *web_get_url(web_context ctx) {
    char response[2048] = {0};
    _rcs(ctx, "/url", NULL, response);
    DEBUG("response (truncated): %.200s", response);

    char *p = strstr(response, "\"value\":\"");
    if (!p) {
        DEBUG("current URL not found in response");
        return NULL;
    }
    p += strlen("\"value\":\"");
    char *end = strchr(p, '"');
    if (!end) {
        DEBUG("malformed response, no closing quote for URL");
        return NULL;
    }
    size_t url_len = end - p;
    char *current_url = (char *) malloc(url_len + 1);
    strncpy(current_url, p, url_len);
    current_url[url_len] = '\0';

    DEBUG("extracted current_url='%s'", current_url);
    return current_url;
}
