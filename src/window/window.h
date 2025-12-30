#ifndef WINDOW_H
#define WINDOW_H

#include "../core/core.h"


/**
 * \brief Get current window handle
 * \param ctx  web context
 * \return current window handle
 */
char *web_get_window(web_context ctx);
/**
 * \brief Close current window
 * \param ctx  web context
 * \return 0 on success, -1 on failure
 */
int web_close_window(web_context ctx);
int web_switch_to_window(web_context ctx, char *handle);
char **web_get_window_handles(web_context ctx);
char *web_new_window(web_context ctx);
char *web_new_tab(web_context ctx);
int web_switch_to_page_content(web_context ctx);
int web_switch_to_tab(web_context ctx, char *tab_index);
int web_switch_to_frame(web_context ctx, char *frame_id);
int web_switch_to_frame_parent(web_context ctx);
typedef struct {
	int x;
	int y;
	int width;
	int height;
} web_window_rect;
web_window_rect web_get_window_rect(web_context ctx);
int web_set_window_rect(web_context ctx, web_window_rect rect);
web_window_rect web_maximize_window(web_context ctx);
web_window_rect web_minimize_window(web_context ctx);
web_window_rect web_fullscreen_window(web_context ctx);

#endif				// WINDOW_H
