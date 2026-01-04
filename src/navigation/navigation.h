#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "../core/core.h"

int web_navigate_to(web_context *ctx, char *url);
char *web_get_url(web_context *ctx);
int web_back(web_context *ctx);
int web_forward(web_context *ctx);
int web_refresh(web_context *ctx);
char *web_get_title(web_context *ctx);
int wait_to_page_load(web_context *ctx, int max_wait_seconds);

#endif				// NAVIGATION_H
