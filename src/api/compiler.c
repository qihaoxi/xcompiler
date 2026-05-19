#include "xcompiler/api/compiler.h"
#include <stdio.h>
#include "core/project_layout.h"
#include "driver/pipeline.h"
#include "platform/platform.h"

const char *xcompiler_version(void)
{
	return XCOMPILER_VERSION_STRING;
}

const xcompiler_framework_snapshot_t *xcompiler_framework_snapshot(void);

const xcompiler_framework_snapshot_t *xcompiler_framework_snapshot(void)
{
	static xcompiler_framework_snapshot_t snapshot;
	static int initialized = 0;
	if (!initialized)
	{
		snapshot.version = xcompiler_version();
		snapshot.modules = xcompiler_build_default_modules(&snapshot.module_count);
		snapshot.pipeline = xcompiler_build_default_pipeline(&snapshot.pipeline_count);
		initialized = 1;
	}
	return &snapshot;
}

size_t xcompiler_framework_describe(char *buffer, size_t capacity)
{
	const xcompiler_framework_snapshot_t *snapshot = xcompiler_framework_snapshot();
	if (!buffer || capacity == 0)
	{
		return 0;
	}
	return (size_t)snprintf(
		buffer,
		capacity,
		"xcompiler %s\nplatform: %s\nbackend: %s\nmodules: %zu\npipeline stages: %zu\n",
		snapshot->version,
		xcompiler_host_platform_family(),
		xcompiler_preferred_backend(),
		snapshot->module_count,
		snapshot->pipeline_count
	);
}
