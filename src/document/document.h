#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "../core/core.h"

/**
 * \brief Execute JavaScript in the context of the currently selected frame or element
 * \param ctx web context
 * \param script JavaScript code to execute
 * \param args arguments to pass to the script (as a JSON array)
 * \param result output result of the script execution (it can be: number, string, array or object)
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_execute_script_sync(web_context *ctx, const char *script, cJSON *args, cJSON **result);
/**
 * \brief Execute JavaScript asynchronously in the context of the currently selected frame or element
 * \param ctx web context
 * \param script JavaScript code to execute
 * \param args arguments to pass to the script (as a JSON array)
 * \param result output result of the script execution (it can be: number, string, array or object)
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_execute_script_async(web_context *ctx, const char *script, cJSON *args, cJSON **result);
void web_dismiss_alert(void);
void web_accept_alert(void);
void web_get_alert_text(void);
void web_send_text_to_alert(void);
/**
 * \brief Get page source
 * \param ctx web context
 * \param source output page source
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_get_page_source(web_context *ctx, char **source);
void web_print_page(void);
void web_perform_actions(void);
void web_release_actions(void);
void web_take_screenshot(void);
void web_take_element_screenshot(void);

#endif				// DOCUMENT_H
