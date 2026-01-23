#ifndef ELEMENT_H
#define ELEMENT_H

#include "../core/core.h"

typedef enum {
    CSS_SELECTOR, //	css selector
    LINK_TEXT_SELECTOR, //	link text
    PARTIAL_LINK_TEXT_SELECTOR, // 	partial link text
    TAG_NAME, //	tag name
    XPATH_SELECTOR //	xpath
} web_element_location_strategy;

/**
 * \brief Get the active element on the page
 * \param ctx web context
 * \param element_id output element ID
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_element_active(web_context *ctx, char **element_id);
/**
 * \brief Get the shadow root of an element
 * \param ctx web context
 * \param element_id element ID
 * \param shadow_root_id output shadow root ID
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_get_element_shadow_root(web_context *ctx, char *element_id, char **shadow_root_id);
/**
 * \brief Find an element on the page
 * \param ctx web context
 * \param strategy location strategy
 * \param selector selector string
 * \param element_id output element ID
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_find_element(web_context *ctx, web_element_location_strategy strategy, char *selector, char **element_id);
/**
 * \brief Find multiple elements on the page
 * \param ctx web context
 * \param strategy location strategy
 * \param selector selector string
 * \param elements_id output array of element IDs, NULL-terminated
 * \return \b int — number of elements \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_find_elements(web_context *ctx, web_element_location_strategy strategy, char *selector, char ***elements_id);
/**
 * \brief Find an element within another element
 * \param ctx web context
 * \param strategy location strategy
 * \param selector selector string
 * \param element_id_src source element ID
 * \param elements_id output element ID
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_find_element_from_element(web_context *ctx, web_element_location_strategy strategy, char *selector, char *element_id_src, char **element_id);
/**
 * \brief Find multiple elements within another element
 * \param ctx web context
 * \param strategy location strategy
 * \param selector selector string
 * \param element_id_src source element ID
 * \param elements_id output array of element IDs, NULL-terminated
 * \return \b int — number of elements \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_find_elements_from_element(web_context *ctx, web_element_location_strategy strategy, char *selector, char *element_id_src, char ***elements_id);
/**
 * \brief Find an element within a shadow root
 * \param ctx web context
 * \param strategy location strategy
 * \param selector selector string, XPath not supported
 * \param shadow_root_id shadow root ID
 * \param element_id output element ID
 * \return \b 2xx http code — Success \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_find_element_from_shadow_root(web_context *ctx, web_element_location_strategy strategy, char *selector, char *shadow_root_id, char **element_id);
/**
 * \brief Find multiple elements within a shadow root
 * \param ctx web context
 * \param strategy location strategy
 * \param selector selector string, XPath not supported
 * \param shadow_root_id shadow root ID
 * \param elements_id output array of element IDs, NULL-terminated
 * \return \b int — number of elements \n\b -xxx http code (negative code) — Failure \n\b -1 — Error
 */
int web_find_elements_from_shadow_root(web_context *ctx, web_element_location_strategy strategy, char *selector, char *shadow_root_id, char ***elements_id);

int web_get_element_attribute(web_context *ctx, char *element_id, char *attribute_name, char **attribute_value);

int web_get_element_property(web_context *ctx, char *element_id, char *property_name, char **property_value);

int web_get_element_css_value(web_context *ctx, char *element_id, char *css_property_name, char **css_property_value);

int web_get_element_text(web_context *ctx, char *element_id, char **text);

void web_get_element_tag_name(void);

void web_get_element_rect(void);

void web_is_element_enabled(void);

void web_is_element_selected(void);


void web_get_computed_role(void);

void web_get_computed_label(void);

void web_click_element(void);

void web_clear_element(void);

void web_send_keys_to_element(void);

#endif				// ELEMENT_H
