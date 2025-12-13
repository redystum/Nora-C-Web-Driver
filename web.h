#ifndef WEB_H
#define WEB_H

#define t_ctx web_context
typedef struct {
	int port;
	char *geckodriverPath;
	char *firefoxPath;
	char *session_id;
} web_context;

web_context web_init(char *geckodriverPath, char *firefoxPath, int port, int force_kill);
int web_open(web_context ctx, char *link);
int web_change_url(web_context ctx, char *link);
char* web_get_current_url(web_context ctx);
int web_close(web_context *ctx);

#endif				// WEB_H
