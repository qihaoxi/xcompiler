#include "xcompiler/api/compiler.h"

#include <stdio.h>

#include "common/status.h"
#include "core/project_layout.h"
#include "driver/pipeline.h"
#include "platform/platform.h"
#include "xcompiler/common/log.h"
#include "xcompiler/ir/ir.h"

const char* xc_version(void) {
	return XC_VERSION_STRING;
}

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

xc_compile_result_t xc_compile(const xc_compiler_config_t* config, const char* source) {
	xc_compile_result_t result = { XC_STATUS_INVALID_ARGUMENT, { NULL, 0 } };

	if (!config || !source) {
		XC_LOG_ERROR("%s", "compile: config or source is NULL");
		return result;
	}

	XC_LOG_INFO("compile: source=%s opt=%d target=%s", config->source_name ? config->source_name : "<string>",
	            config->optimization_level, config->target_triple);

	/* --- stage 1: lexing --- */
	xc_lexer_t lexer;
	xc_lexer_init(&lexer, source);
	if (!xc_lexer_is_ready(&lexer)) {
		result.status = XC_STATUS_INTERNAL_ERROR;
		return result;
	}
	XC_LOG_DEBUG("%s", "lexing stage: ready");

	/* --- stage 2: parsing --- */
	xc_parser_t parser;
	xc_parser_init(&parser, &lexer);
	if (!xc_parser_is_ready(&parser)) {
		result.status = XC_STATUS_INTERNAL_ERROR;
		return result;
	}

	xc_ast_t ast;
	result.status = xc_parser_parse(&parser, &ast);
	if (result.status != XC_STATUS_OK) {
		XC_LOG_WARN("parsing stage: failed with status %s", xc_status_to_string(result.status));
		return result;
	}
	XC_LOG_DEBUG("parsing stage: %zu nodes", ast.node_count);
	result.ast = ast;

	/* --- stage 3: semantic analysis --- */
	xc_semantic_context_t sem_ctx;
	xc_semantic_context_init(&sem_ctx, &ast);
	if (!xc_semantic_context_is_ready(&sem_ctx)) {
		result.status = XC_STATUS_INTERNAL_ERROR;
		return result;
	}

	result.status = xc_semantic_analyze(&sem_ctx);
	if (result.status != XC_STATUS_OK) {
		XC_LOG_WARN("semantic stage: failed with status %s", xc_status_to_string(result.status));
		return result;
	}
	XC_LOG_DEBUG("%s", "semantic stage: ok");

	/* --- stage 4: ir lowering --- */
	xc_ir_module_t ir_module;
	xc_ir_module_init(&ir_module, config->source_name ? config->source_name : "main", &ast);
	if (!xc_ir_module_is_ready(&ir_module)) {
		result.status = XC_STATUS_INTERNAL_ERROR;
		return result;
	}

	result.status = xc_ir_module_lower(&ir_module);
	if (result.status == XC_STATUS_NOT_READY) {
		XC_LOG_DEBUG("%s", "ir stage: not yet implemented, skipping");
		result.status = XC_STATUS_OK;
	} else if (result.status != XC_STATUS_OK) {
		XC_LOG_WARN("ir stage: failed with status %s", xc_status_to_string(result.status));
		return result;
	}
	XC_LOG_DEBUG("%s", "ir stage: ok");

	XC_LOG_INFO("%s", "compile: done");
	return result;
}
