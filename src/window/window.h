#ifndef WINDOW_H
#define WINDOW_H

#include "../core/core.h"


/**
 * \brief Get current window handle
 * \param ctx web context
 * \param handle output window handle
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_get_window(web_context *ctx, char **handle) ;
/**
 * \brief Close current window
 * \param ctx web context
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_close_window(web_context *ctx);
/**
 * \brief Close current tab
 * \param ctx web context
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_close_tab(web_context *ctx);
/**
 * \brief Switch to window by handle
 * \param ctx web context
 * \param handle window handle
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_switch_to_window(web_context *ctx, char *handle);
/**
 * \brief Switch to tab by handle
 * \param ctx web context
 * \param handle tab handle
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_switch_to_tab(web_context *ctx, char *handle);
/**
 * \brief Get all window handles
 * \param ctx web context
 * \param handles output array of window handles, NULL-terminated
 * \return \b int — number of handles \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_get_window_handles(web_context *ctx, char ***handles);
/***
 * \brief Create a new window
 * \param ctx web context
 * \param handle output new window handle
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_new_window(web_context *ctx, char **handle);
/**
 * \brief Create a new tab
 * \param ctx web context
 * \param handle output new tab handle
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_new_tab(web_context *ctx, char **handle) ;
/**
 * \brief Switch to page content (default frame)
 * \param ctx web context
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_switch_to_page_content(web_context *ctx);
/**
 * \brief Switch to frame by id or name
 * \param ctx web context
 * \param frame_id frame id or name
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_switch_to_frame(web_context *ctx, char *frame_id);
/**
 * \brief Switch to parent frame
 * \param ctx web context
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_switch_to_frame_parent(web_context *ctx);

typedef struct {
    int x;
    int y;
    int width;
    int height;
} web_window_rect;

/**
 * \brief Get current window rectangle
 * \param ctx web context
 * \param rect output window rectangle
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_get_window_rect(web_context *ctx, web_window_rect *rect);
/**
 * \brief Set current window rectangle
 * \param ctx web context
 * \param rect input window rectangle
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_set_window_rect(web_context *ctx, web_window_rect rect);
/**
 * \brief Maximize current window
 * \param ctx web context
 * \param rect output window rectangle after maximizing
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_maximize_window(web_context *ctx, web_window_rect *rect);
/**
 * \brief Minimize current window
 * \param ctx web context
 * \param rect output window rectangle after minimizing
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_minimize_window(web_context *ctx, web_window_rect *rect);
/**
 * \brief Set current window to fullscreen
 * \param ctx web context
 * \param rect output window rectangle after fullscreen
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_fullscreen_window(web_context *ctx, web_window_rect *rect);

#endif				// WINDOW_H
