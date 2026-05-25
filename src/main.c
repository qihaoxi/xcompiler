#include "xcompiler/api/compiler.h"
#include "xcompiler/common/log.h"
#include <stdio.h>
#include <string.h>

static void print_usage(const char* program) {
	printf("Usage: %s [--version|--describe]\n", program);
}

int main(int argc, char** argv) {
	xc_log_init_console(XC_LOG_LEVEL_INFO);
	XC_LOG_DEBUG("%s", "CLI starting");

	if (argc > 1 && strcmp(argv[1], "--version") == 0) {
		puts(xc_version());
		xc_log_shutdown();
		return 0;
	}
	if (argc > 1 && strcmp(argv[1], "--describe") == 0) {
		char buffer[512];
		xc_framework_describe(buffer, sizeof(buffer));
		fputs(buffer, stdout);
		xc_log_shutdown();
		return 0;
	}
	print_usage(argv[0]);
	xc_log_shutdown();
	return 0;
}
