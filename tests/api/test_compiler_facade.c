#include "xcompiler/api/compiler.h"
#include <assert.h>
#include <string.h>

int main(void)
{
	const xcompiler_framework_snapshot_t *snapshot = xcompiler_framework_snapshot();
	assert(snapshot != NULL);
	assert(snapshot->version != NULL);
	assert(snapshot->module_count >= 5);
	assert(snapshot->pipeline_count >= 4);
	assert(strcmp(snapshot->modules[0].name, "common") == 0);
	return 0;
}
