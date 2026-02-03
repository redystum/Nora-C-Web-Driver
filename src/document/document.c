#include <stdio.h>
#include "document.h"

#include "communication_internal/communication.h"

int web_get_page_source(web_context *ctx, char **source) {
    CHECK_NULL(ctx, ctx);

    cJSON *response_json = NULL;
    int resp = RCS(ctx, "/source", NULL, &response_json, WEB_GET);
    DEBUG_JSON(response_json);
    DEBUG("%d", resp);
    if (resp < 0) {
        return resp;
    }

    cJSON *value = cJSON_GetObjectItemCaseSensitive(response_json, "value");
    *source = strdup(value->valuestring);

    cJSON_Delete(response_json);
    return resp;
}
