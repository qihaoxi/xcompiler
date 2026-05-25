#include "xcompiler/common/log.h"

int main(void) {
	xc_log_init_console(XC_LOG_LEVEL_INFO);
	XC_LOG_INFO("%s", "logging demo ready");
	xc_log_shutdown();
	return 0;
}
