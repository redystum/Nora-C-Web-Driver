#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "../core/core.h"

/**
 * \brief Navigate to a URL
 * \param ctx web context
 * \param url URL to navigate to (or NULL for about:blank)
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_navigate_to(web_context *ctx, char *url);
/**
 * \brief Get current URL
 * \param ctx web context
 * \param url output current URL
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_get_url(web_context *ctx, char **url);
/**
 * \brief Navigate back in history
 * \param ctx web context
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_back(web_context *ctx);
/**
 * \brief Navigate forward in history
 * \param ctx web context
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_forward(web_context *ctx);
/**
 * \brief Refresh current page
 * \param ctx web context
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_refresh(web_context *ctx);
/**
 * \brief Get current page title
 * \param ctx web context
 * \param title output page title
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_get_title(web_context *ctx, char **title);
/**
 * \brief Wait for page to load completely
 * \param ctx web context
 * \param max_wait_seconds maximum wait time in seconds (0 for infinite)
 * \return \b 0 — Page loaded successfully \n\b -1 — Timeout or Error
 */
int wait_to_page_load(web_context *ctx, int max_wait_seconds);

#endif				// NAVIGATION_H
