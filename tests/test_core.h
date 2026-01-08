#ifndef TEST_CORE_H
#define TEST_CORE_H

#include "test_common.h"
#include "../src/communication_internal/communication.h" // For RCS and WEB_POST

void test_errors(web_context *ctx);
void test_timeouts(web_context *ctx);

#endif

