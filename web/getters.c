#include "web.h"


char* web_get_current_url(web_context ctx) {
    char response[2048] = { 0 };
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
    char *current_url = (char *)malloc(url_len + 1);
    strncpy(current_url, p, url_len);
    current_url[url_len] = '\0';

    DEBUG("extracted current_url='%s'", current_url);
    return current_url;
}
