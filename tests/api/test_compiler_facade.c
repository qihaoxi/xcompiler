#include "xcompiler/api/compiler.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

static void expect_module_dependencies(const xc_module_descriptor_t* module, const char* const* expected,
                                       size_t count) {
	assert(module->dependency_count == count);
	for (size_t i = 0; i < count; ++i) {
		assert(strcmp(module->dependencies[i], expected[i]) == 0);
	}
}

static void expect_stage_modules(const xc_stage_descriptor_t* stage, const char* const* expected, size_t count) {
	assert(stage->module_count == count);
	for (size_t i = 0; i < count; ++i) {
		assert(strcmp(stage->modules[i], expected[i]) == 0);
	}
}

static void expect_modules(const xc_framework_snapshot_t* snapshot) {
	static const char* const* k_common_deps            = NULL;
	static const char* const  k_platform_deps[]        = { "common" };
	static const char* const  k_api_deps[]             = { "common", "platform" };
	static const char* const  k_frontend_common_deps[] = { "common" };
	static const char* const  k_lexer_deps[]           = { "common", "frontend/common" };
	static const char* const  k_ast_deps[]             = { "common", "frontend/common" };
	static const char* const  k_parser_deps[]   = { "common", "frontend/common", "frontend/lexer", "frontend/ast" };
	static const char* const  k_semantic_deps[] = { "common", "frontend/ast" };
	static const char* const  k_ir_deps[]       = { "common", "frontend/semantic" };
	static const char* const  k_driver_deps[]   = { "api", "frontend/parser", "frontend/semantic", "ir" };

	assert(snapshot->module_count == 10);
	assert(strcmp(snapshot->modules[0].name, "common") == 0);
	assert(strcmp(snapshot->modules[0].responsibility, "Shared status and diagnostic primitives.") == 0);
	expect_module_dependencies(&snapshot->modules[0], k_common_deps, 0);

	assert(strcmp(snapshot->modules[1].name, "platform") == 0);
	assert(strcmp(snapshot->modules[1].responsibility, "Platform and backend selection helpers.") == 0);
	expect_module_dependencies(&snapshot->modules[1], k_platform_deps, 1);

	assert(strcmp(snapshot->modules[2].name, "api") == 0);
	assert(strcmp(snapshot->modules[2].responsibility, "Stable external facade for embedding and CLI use.") == 0);
	expect_module_dependencies(&snapshot->modules[2], k_api_deps, 2);

	assert(strcmp(snapshot->modules[3].name, "frontend/common") == 0);
	assert(strcmp(snapshot->modules[3].responsibility, "Shared source location and text-view primitives.") == 0);
	expect_module_dependencies(&snapshot->modules[3], k_frontend_common_deps, 1);

	assert(strcmp(snapshot->modules[4].name, "frontend/lexer") == 0);
	assert(strcmp(snapshot->modules[4].responsibility, "Token stream and source slicing boundaries.") == 0);
	expect_module_dependencies(&snapshot->modules[4], k_lexer_deps, 2);

	assert(strcmp(snapshot->modules[5].name, "frontend/ast") == 0);
	assert(strcmp(snapshot->modules[5].responsibility, "Tree ownership and traversal model.") == 0);
	expect_module_dependencies(&snapshot->modules[5], k_ast_deps, 2);

	assert(strcmp(snapshot->modules[6].name, "frontend/parser") == 0);
	assert(strcmp(snapshot->modules[6].responsibility, "Syntax tree construction boundary.") == 0);
	expect_module_dependencies(&snapshot->modules[6], k_parser_deps, 4);

	assert(strcmp(snapshot->modules[7].name, "frontend/semantic") == 0);
	assert(strcmp(snapshot->modules[7].responsibility, "Symbol, scope, and type-checking extension point.") == 0);
	expect_module_dependencies(&snapshot->modules[7], k_semantic_deps, 2);

	assert(strcmp(snapshot->modules[8].name, "ir") == 0);
	assert(strcmp(snapshot->modules[8].responsibility, "Lowering boundary before LLVM integration.") == 0);
	expect_module_dependencies(&snapshot->modules[8], k_ir_deps, 2);

	assert(strcmp(snapshot->modules[9].name, "driver") == 0);
	assert(strcmp(snapshot->modules[9].responsibility, "Pipeline orchestration and entrypoint glue.") == 0);
	expect_module_dependencies(&snapshot->modules[9], k_driver_deps, 4);
}

static void expect_pipeline(const xc_framework_snapshot_t* snapshot) {
	static const char* const k_lexing_modules[]   = { "frontend/common", "frontend/lexer" };
	static const char* const k_parsing_modules[]  = { "frontend/common", "frontend/lexer", "frontend/ast",
		                                              "frontend/parser" };
	static const char* const k_semantic_modules[] = { "frontend/semantic" };
	static const char* const k_ir_modules[]       = { "ir" };
	static const char* const k_driver_modules[]   = { "driver", "api" };

	assert(snapshot->pipeline_count == 5);
	assert(strcmp(snapshot->pipeline[0].name, "lexing") == 0);
	assert(strcmp(snapshot->pipeline[0].responsibility, "Convert source text into a token stream boundary.") == 0);
	expect_stage_modules(&snapshot->pipeline[0], k_lexing_modules, 2);

	assert(strcmp(snapshot->pipeline[1].name, "parsing") == 0);
	assert(strcmp(snapshot->pipeline[1].responsibility, "Build the AST and keep syntax ownership isolated.") == 0);
	expect_stage_modules(&snapshot->pipeline[1], k_parsing_modules, 4);

	assert(strcmp(snapshot->pipeline[2].name, "semantic-analysis") == 0);
	assert(strcmp(snapshot->pipeline[2].responsibility, "Attach symbols, scopes, and basic type checks.") == 0);
	expect_stage_modules(&snapshot->pipeline[2], k_semantic_modules, 1);

	assert(strcmp(snapshot->pipeline[3].name, "ir-lowering") == 0);
	assert(strcmp(snapshot->pipeline[3].responsibility, "Lower validated frontend state into an IR boundary.") == 0);
	expect_stage_modules(&snapshot->pipeline[3], k_ir_modules, 1);

	assert(strcmp(snapshot->pipeline[4].name, "driver-orchestration") == 0);
	assert(strcmp(snapshot->pipeline[4].responsibility, "Coordinate CLI, embedding API, and pipeline composition.") ==
	       0);
	expect_stage_modules(&snapshot->pipeline[4], k_driver_modules, 2);
}

int main(void) {
	const xc_framework_snapshot_t* snapshot = xc_framework_snapshot();
	assert(snapshot != NULL);
	assert(snapshot->version != NULL);
	assert(strcmp(snapshot->version, xc_version()) == 0);
	expect_modules(snapshot);
	expect_pipeline(snapshot);
	return 0;
}
