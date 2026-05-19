#pragma once

#include <stddef.h>

#include "xcompiler/api/version.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xcompiler_module_descriptor
{
	const char *name;
	const char *responsibility;
	const char *const *dependencies;
	size_t dependency_count;
} xcompiler_module_descriptor_t;

typedef struct xcompiler_stage_descriptor
{
	const char *name;
	const char *responsibility;
	const char *const *modules;
	size_t module_count;
} xcompiler_stage_descriptor_t;

typedef struct xcompiler_framework_snapshot
{
	const char *version;
	const xcompiler_module_descriptor_t *modules;
	size_t module_count;
	const xcompiler_stage_descriptor_t *pipeline;
	size_t pipeline_count;
} xcompiler_framework_snapshot_t;

const char *xcompiler_version(void);
const xcompiler_framework_snapshot_t *xcompiler_framework_snapshot(void);
size_t xcompiler_framework_describe(char *buffer, size_t capacity);

#ifdef __cplusplus
}
#endif
