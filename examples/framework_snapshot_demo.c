#include "xcompiler/api/compiler.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

int main(void) {
	const xc_framework_snapshot_t* snapshot = xc_framework_snapshot();
	assert(snapshot != NULL);

	printf("xcompiler framework snapshot\n");
	printf("version: %s\n", xc_version());
	printf("modules (%zu):\n", snapshot->module_count);
	for (size_t i = 0; i < snapshot->module_count; ++i) {
		const xc_module_descriptor_t* module = &snapshot->modules[i];
		printf("  - %s: %s\n", module->name, module->responsibility);
	}

	printf("pipeline stages (%zu):\n", snapshot->pipeline_count);
	for (size_t i = 0; i < snapshot->pipeline_count; ++i) {
		const xc_stage_descriptor_t* stage = &snapshot->pipeline[i];
		printf("  - %s: %s\n", stage->name, stage->responsibility);
	}

	return 0;
}
