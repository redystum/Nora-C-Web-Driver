#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "../core/core.h"

// typedef struct {
//     char* name;
//     char* value;
// } web_attribute;
//
// typedef struct {
//     // just the most important attributes here
//     char tag_name[64];
//     char *element_id;
//     char *class_name;
//     web_attribute attributes[255];
// } web_element;

void web_execute_script(void);
void web_execute_async_script(void);
void web_dismiss_alert(void);
void web_accept_alert(void);
void web_get_alert_text(void);
void web_send_text_to_alert(void);
void web_get_page_source(void);
void web_print_page(void);
void web_perform_actions(void);
void web_release_actions(void);
void web_take_screenshot(void);
void web_take_element_screenshot(void);

#endif				// DOCUMENT_H
