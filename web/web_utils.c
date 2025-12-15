#include "web.h"

int web_usleep(int microseconds) {
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    return nanosleep(&ts, NULL);
}