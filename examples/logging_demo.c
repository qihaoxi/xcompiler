#include "../include/xcompiler/common/xlog.h"

int main(void)
{
	xlog_init_console(XLOG_LEVEL_INFO);
	XLOG_INFO("%s", "logging demo ready");
	xlog_shutdown();
	return 0;
}
