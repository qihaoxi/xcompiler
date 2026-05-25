#include "xcompiler/api/compiler.h"
#include <stdio.h>
#include "core/project_layout.h"
#include "driver/pipeline.h"
#include "platform/platform.h"

const char* xc_version(void) {
	return XC_VERSION_STRING;
}

const xc_framework_snapshot_t* xc_framework_snapshot(void);

const xc_framework_snapshot_t* xc_framework_snapshot(void) {
	static xc_framework_snapshot_t snapshot;
	static int                     initialized = 0;
	if (!initialized) {
		snapshot.version  = xc_version();
		snapshot.modules  = xc_build_default_modules(&snapshot.module_count);
		snapshot.pipeline = xc_build_default_pipeline(&snapshot.pipeline_count);
		initialized       = 1;
	}
	return &snapshot;
}

size_t xc_framework_describe(char* buffer, size_t capacity) {
	const xc_framework_snapshot_t* snapshot = xc_framework_snapshot();
	if (!buffer || capacity == 0) {
		return 0;
	}
	return (size_t)snprintf(buffer, capacity,
	                        "xcompiler %s\nplatform: %s\nbackend: %s\nmodules: %zu\npipeline stages: %zu\n",
	                        snapshot->version, xc_host_platform_family(), xc_preferred_backend(),
	                        snapshot->module_count, snapshot->pipeline_count);
}
