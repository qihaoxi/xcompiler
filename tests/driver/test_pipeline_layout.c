#include "core/project_layout.h"
#include "driver/pipeline.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

int main(void)
{
	size_t module_count = 0;
	const xcompiler_module_descriptor_t *modules = xcompiler_build_default_modules(&module_count);
	assert(modules != NULL);
	assert(module_count >= 5);
	assert(strcmp(modules[1].name, "platform") == 0);
	size_t stage_count = 0;
	const xcompiler_stage_descriptor_t *pipeline = xcompiler_build_default_pipeline(&stage_count);
	assert(pipeline != NULL);
	assert(stage_count >= 4);
	assert(strcmp(pipeline[0].name, "lexing") == 0);
	assert(strcmp(pipeline[stage_count - 1].name, "driver-orchestration") == 0);
	return 0;
}
