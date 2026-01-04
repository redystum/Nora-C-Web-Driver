#ifndef WINDOW_H
#define WINDOW_H

#include "../core/core.h"


/**
 * \brief Get current window handle
 * \param ctx web context
 * \return current window handle
 */
char *web_get_window(web_context *ctx);

/**
 * \brief Close current window
 * \param ctx web context
 * \return 0 on success, -1 on failure
 */
int web_close_window(web_context *ctx);

/**
 * \brief Close current tab
 * \param ctx web context
 * \return 0 on success, -1 on failure
 */
int web_close_tab(web_context *ctx);

/**
 * \brief Switch to window by handle
 * \param ctx web context
 * \param handle  window handle
 * \return 0 on success, -1 on failure
 */
int web_switch_to_window(web_context *ctx, char *handle);

/**
 * \brief Switch to tab by handle
 * \param ctx web context
 * \param handle  tab handle
 * \return 0 on success, -1 on failure
 */
int web_switch_to_tab(web_context *ctx, char *handle);

/**
 * \brief Get all window handles
 * \param ctx web context
 * \return array of window handles (NULL-terminated)
 */
char **web_get_window_handles(web_context *ctx);

/**
 * \brief Create a new window
 * \param ctx web context
 * \return new window handle
 */
char *web_new_window(web_context *ctx);

/**
 * \brief Create a new tab
 * \param ctx web context
 * \return new tab handle
 */
char *web_new_tab(web_context *ctx);

/**
 * \brief Switch to page content (default frame)
 * \param ctx web context
 * \return 0 on success, -1 on failure
 */
int web_switch_to_page_content(web_context *ctx);

/**
 * \brief Switch to frame by id
 * \param ctx web context
 * \param frame_id  frame id or NULL for default content
 * \return 0 on success, -1 on failure
 */
int web_switch_to_frame(web_context *ctx, char *frame_id);

/**
 * \brief Switch to parent frame
 * \param ctx web context
 * \return 0 on success, -1 on failure
 */
int web_switch_to_frame_parent(web_context *ctx);

typedef struct {
    int x;
    int y;
    int width;
    int height;
} web_window_rect;

web_window_rect web_get_window_rect(web_context *ctx);

int web_set_window_rect(web_context *ctx, web_window_rect rect);

web_window_rect web_maximize_window(web_context *ctx);

web_window_rect web_minimize_window(web_context *ctx);

web_window_rect web_fullscreen_window(web_context *ctx);

#endif				// WINDOW_H
