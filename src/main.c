#include "xcompiler/api/compiler.h"
#include "xcompiler/api/config.h"
#include "xcompiler/common/log.h"

#include <stdio.h>
#include <string.h>

static void print_usage(const char* program) {
	printf("Usage: %s [--version|--describe|--compile <source>]\n", program);
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
	if (argc > 2 && strcmp(argv[1], "--compile") == 0) {
		xc_compiler_config_t cfg;
		xc_compiler_config_init(&cfg);
		cfg.source_name       = "<cmdline>";
		cfg.optimization_level = 1;

		xc_compile_result_t result = xc_compile(&cfg, argv[2]);
		printf("compile status: %s\n", xc_status_to_string(result.status));
		printf("ast nodes: %zu\n", result.ast.node_count);
		xc_log_shutdown();
		return result.status == XC_STATUS_OK ? 0 : 1;
	}

	print_usage(argv[0]);
	xc_log_shutdown();
	return 0;
}
