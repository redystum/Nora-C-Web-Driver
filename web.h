#ifndef WEB_H
#define WEB_H

#define t_ctx web_context
typedef struct {
	char *geckodriverPath;
	char *firefoxPath;
	char *session_id;
	char *current_url;
} web_context;

web_context web_init(char *geckodriverPath, char *firefoxPath);
int web_open(web_context ctx, char *link);
int web_close(web_context *ctx);

#endif				// WEB_H
