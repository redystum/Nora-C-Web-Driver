#ifndef WEB_H
#define WEB_H

int web_init(char *geckodriverPath, char *firefoxPath);
int web_open(char *link);
int web_close(void);

#endif				// WEB_H
