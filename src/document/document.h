#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "../core/core.h"

void web_execute_script(void);
void web_execute_async_script(void);
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
