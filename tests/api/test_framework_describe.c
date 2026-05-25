#include "xcompiler/api/compiler.h"

#include "platform/platform.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void build_expected_description(char* buffer, size_t capacity, const xc_framework_snapshot_t* snapshot) {
	int rc = snprintf(buffer, capacity, "xcompiler %s\nplatform: %s\nbackend: %s\nmodules: %zu\npipeline stages: %zu\n",
	                  snapshot->version, xc_host_platform_family(), xc_preferred_backend(), snapshot->module_count,
	                  snapshot->pipeline_count);
	assert(rc >= 0);
}

int main(void) {
	const xc_framework_snapshot_t* snapshot = xc_framework_snapshot();
	assert(snapshot != NULL);
	assert(snapshot->version != NULL);
	assert(strcmp(snapshot->version, xc_version()) == 0);
	assert(snapshot->module_count == 10);
	assert(snapshot->pipeline_count == 5);
	assert(strcmp(snapshot->modules[0].name, "common") == 0);
	assert(strcmp(snapshot->modules[snapshot->module_count - 1].name, "driver") == 0);
	assert(strcmp(snapshot->pipeline[0].name, "lexing") == 0);
	assert(strcmp(snapshot->pipeline[snapshot->pipeline_count - 1].name, "driver-orchestration") == 0);

	assert(xc_framework_describe(NULL, 0) == 0);
	assert(xc_framework_describe(NULL, 128) == 0);

	char expected[512];
	build_expected_description(expected, sizeof(expected), snapshot);
	const size_t expected_length = strlen(expected);

	char   buffer[512];
	size_t written = xc_framework_describe(buffer, sizeof(buffer));
	assert(written == expected_length);
	assert(strcmp(buffer, expected) == 0);
	assert(buffer[written] == '\0');

	char   tiny[1]      = { 'X' };
	size_t tiny_written = xc_framework_describe(tiny, sizeof(tiny));
	assert(tiny_written == expected_length);
	assert(tiny[0] == '\0');

	return 0;
}
