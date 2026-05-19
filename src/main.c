#include "../include/xcompiler/api/compiler.h"
#include "../include/xcompiler/common/xlog.h"
#include <stdio.h>
#include <string.h>

static void print_usage(const char *program)
{
	printf("Usage: %s [--version|--describe]\n", program);
}

int main(int argc, char **argv)
{
	xlog_init_console(XLOG_LEVEL_ERROR);
	XLOG_DEBUG("%s", "CLI starting");

	if (argc > 1 && strcmp(argv[1], "--version") == 0)
	{
		puts(xcompiler_version());
		xlog_shutdown();
		return 0;
	}
	if (argc > 1 && strcmp(argv[1], "--describe") == 0)
	{
		char buffer[512];
		xcompiler_framework_describe(buffer, sizeof(buffer));
		fputs(buffer, stdout);
		xlog_shutdown();
		return 0;
	}
	print_usage(argv[0]);
	xlog_shutdown();
	return 0;
}
