#pragma once

#include <stddef.h>

#include "xcompiler/api/config.h"
#include "xcompiler/api/version.h"
#include "xcompiler/frontend/frontend.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xc_module_descriptor {
	const char*        name;
	const char*        responsibility;
	const char* const* dependencies;
	size_t             dependency_count;
} xc_module_descriptor_t;

typedef struct xc_stage_descriptor {
	const char*        name;
	const char*        responsibility;
	const char* const* modules;
	size_t             module_count;
} xc_stage_descriptor_t;

typedef struct xc_framework_snapshot {
	const char*                   version;
	const xc_module_descriptor_t* modules;
	size_t                        module_count;
	const xc_stage_descriptor_t*  pipeline;
	size_t                        pipeline_count;
} xc_framework_snapshot_t;

typedef struct xc_compile_result {
	xc_status_code_t status;
	xc_ast_t         ast;
} xc_compile_result_t;

const char*                    xc_version(void);
const xc_framework_snapshot_t* xc_framework_snapshot(void);
size_t                         xc_framework_describe(char* buffer, size_t capacity);
xc_compile_result_t            xc_compile(const xc_compiler_config_t* config, const char* source);

#ifdef __cplusplus
}
#endif
