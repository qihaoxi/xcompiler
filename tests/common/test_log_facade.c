#include "xcompiler/common/log.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void) {
	remove("./xc_log_facade_smoke.log");
	assert(xc_log_init_console(XC_LOG_LEVEL_ERROR));
	assert(xc_log_is_initialized());
	assert(xc_log_get_level() == XC_LOG_LEVEL_ERROR);

	xc_log_set_level(XC_LOG_LEVEL_DEBUG);
	assert(xc_log_level_enabled(XC_LOG_LEVEL_ERROR));

	xc_log_shutdown();
	assert(!xc_log_is_initialized());

	assert(xc_log_init_file(".", "xc_log_facade_smoke", XC_LOG_LEVEL_INFO));
	XC_LOG_INFO("%s", "hello facade");
	xc_log_flush();
	xc_log_shutdown();

	FILE* fp = fopen("./xc_log_facade_smoke.log", "r");
	assert(fp != NULL);
	char   buffer[1024] = { 0 };
	size_t n            = fread(buffer, 1, sizeof(buffer) - 1, fp);
	buffer[n]           = '\0';
	assert(strstr(buffer, "tests/common/test_log_facade.c") != NULL);
	fclose(fp);

	xc_log_init_console(XC_LOG_LEVEL_INFO);
	XC_LOG_INFO("%s", "logging facade ready");
	xc_log_shutdown();
	assert(!xc_log_is_initialized());
	return 0;
}
