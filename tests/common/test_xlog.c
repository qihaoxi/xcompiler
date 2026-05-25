#include "common/xlog.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
	assert(xlog_init_console(XLOG_LEVEL_ERROR));
	assert(xlog_is_initialized());
	assert(xlog_get_level() == XLOG_LEVEL_ERROR);
	xlog_set_level(XLOG_LEVEL_DEBUG);
	assert(xlog_level_enabled(XLOG_LEVEL_ERROR));
	xlog_shutdown();
	assert(!xlog_is_initialized());
	assert(xlog_init_file(".", "xc_xlog_smoke", XLOG_LEVEL_INFO));
	XLOG_INFO("hello %s", "logger");
	xlog_flush();
	xlog_shutdown();
	FILE* fp = fopen("./xc_xlog_smoke.log", "r");
	assert(fp != NULL);
	fclose(fp);
	return 0;
}
