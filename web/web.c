#include "web.h"


/**
 * \brief Reset web context mantaining paths and port
 * \param ctx  web context
 */
void reset_web_context(t_ctx* ctx) {
	if (ctx->session_id) {
		free(ctx->session_id);
		ctx->session_id = NULL;
	}
}

/**
 * \brief Initialize web context
 * \param geckodriverPath NULL = "geckodriver"
 * \param firefoxPath NULL = "firefox"
 * \param port 0 = 9515
 * \param force_kill 1 = kill existing geckodriver on port
 * \return web context
 */
web_context web_init(char *geckodriverPath, char *firefoxPath, int port, int force_kill) {
	t_ctx ctx = {0};
	if (geckodriverPath != NULL) {
		ctx.geckodriverPath = geckodriverPath;
	} else {
		ctx.geckodriverPath = "geckodriver";
	}
	if (firefoxPath != NULL) {
		ctx.firefoxPath = firefoxPath;
	} else {
		ctx.firefoxPath = "firefox";
	}
	if (port != 0) {
		ctx.port = port;
	} else {
		ctx.port = 9515;
	}

	if (_gecko_run(ctx, force_kill) < 0) {
		DEBUG("Failed to start geckodriver");
		reset_web_context(&ctx);
		return ctx;
	}

	if (_wait_for_gecko_ready(&ctx) < 0) {
		DEBUG("Geckodriver failed to start");
		reset_web_context(&ctx);
		return ctx;
	}

	DEBUG("extracted session_id='%s'", ctx.session_id);

	return ctx;
}

/**
 * \brief Open link in browser
 * \param ctx web context
 * \param link link to open
 * \return 0=ok
 */
int web_open(web_context ctx, char *link) {
	char response[2048] = { 0 };
	char *data = NULL;
	ut_str_cat(&data, "{\"url\": \"", link, "\"}", NULL);
	DEBUG("opening link='%s' data='%s'", link ? link : "(null)",
	      data ? data : "(null)");
	_rcs(ctx, "/url", data, response);
	DEBUG("response (truncated): %.200s", response);
	free(data);
	return 0;
}

/**
 * \brief Change current URL
 * \param ctx web context
 * \param link new link
 * \return 0=ok
 */
int web_change_url(web_context ctx, char *link) {
	char response[2048] = { 0 };
	char *data = NULL;
	ut_str_cat(&data, "{\"url\": \"", link, "\"}", NULL);
	DEBUG("changing url to link='%s' data='%s'", link ? link : "(null)",
	      data ? data : "(null)");
	_rcs(ctx, "/url", data, response);
	DEBUG("response (truncated): %.200s", response);
	free(data);
	return 0;
}

/**
 * \brief Close web session
 * \param ctx  web context
 * \return 0=ok
 */
int web_close(web_context *ctx) {
	DEBUG("closing session '%s'", ctx->session_id);
	CURL *curl = curl_easy_init();

	char endpoint[256];
	sprintf(endpoint, "http://127.0.0.1:%d/session/%s", ctx->port, ctx->session_id);
	DEBUG("endpoint='%s'", endpoint);

	curl_easy_setopt(curl, CURLOPT_URL, endpoint);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	reset_web_context(ctx);

	return 0;
}
