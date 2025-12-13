#include "web.h"

int _gecko_run(t_ctx ctx, int force_kill) {
    if (force_kill) {
        char kill_cmd[256];
        sprintf(kill_cmd, "fuser -k -n tcp %d > /dev/null 2>&1", ctx.port);
        int res = system(kill_cmd);
        DEBUG("killed existing geckodriver on port %d, res=%d",
              ctx.port, res);
        sleep(1);
    }
    char *cmd = NULL;

    char command[1024];
    sprintf(command, "--port %d --binary %s > /dev/null 2>&1 &", ctx.port, ctx.firefoxPath);
    DEBUG("starting geckodriver with command fragment: %s", command);

    ut_str_cat(&cmd, ctx.geckodriverPath, " ", command, NULL);
    DEBUG("executing command: %s", cmd ? cmd : "(null)");
    int res = system(cmd);
    DEBUG("geckodriver start command returned %d", res);
    if (cmd) free(cmd);
    return res;
}

int _wait_for_gecko_ready(t_ctx* ctx) {
    char response[2048] = { 0 };
    int max_retries = 30;
    int retry = 0;

    while (retry < max_retries) {
        sleep(1);
        memset(response, 0, sizeof(response));
        _run_curl(*ctx, "/session",
             "{\"capabilities\": {\"alwaysMatch\": {\"browserName\": \"firefox\"}}}",
             response);

        char *p = strstr(response, "\"sessionId\"");
        if (p != NULL) {
            DEBUG("geckodriver is ready");
            if (!p) {
                DEBUG("sessionId not found in response");
                return -2;
            }
            p = strchr(p, ':') + 2;
            char *end = strchr(p, '"');
            char session_id[128];
            strncpy(session_id, p, end - p);
            session_id[end - p] = 0;
            ctx->session_id = strdup(session_id);
            return 0;
        }
        retry++;
        DEBUG("geckodriver not ready yet, retry %d/%d",
              retry, max_retries);

        sleep(1);
    }

    DEBUG("geckodriver failed to start after %d attempts", max_retries);
    return -1;
}
