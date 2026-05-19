#include "core/project_layout.h"
static const char *const k_common_deps[] = {NULL};
static const char *const k_platform_deps[] = {"common"};
static const char *const k_api_deps[] = {"common", "platform"};
static const char *const k_frontend_common_deps[] = {"common"};
static const char *const k_lexer_deps[] = {"common", "frontend/common"};
static const char *const k_ast_deps[] = {"common", "frontend/common"};
static const char *const k_parser_deps[] = {"common", "frontend/common", "frontend/lexer", "frontend/ast"};
static const char *const k_semantic_deps[] = {"common", "frontend/ast"};
static const char *const k_ir_deps[] = {"common", "frontend/semantic"};
static const char *const k_driver_deps[] = {"api", "frontend/parser", "frontend/semantic", "ir"};
static const xcompiler_module_descriptor_t k_modules[] = {
	{"common", "Shared status and diagnostic primitives.", k_common_deps, 0},
	{"platform", "Platform and backend selection helpers.", k_platform_deps, 1},
	{"api", "Stable external facade for embedding and CLI use.", k_api_deps, 2},
	{"frontend/common", "Shared source location and text-view primitives.", k_frontend_common_deps, 1},
	{"frontend/lexer", "Token stream and source slicing boundaries.", k_lexer_deps, 2},
	{"frontend/ast", "Tree ownership and traversal model.", k_ast_deps, 2},
	{"frontend/parser", "Syntax tree construction boundary.", k_parser_deps, 4},
	{"frontend/semantic", "Symbol, scope, and type-checking extension point.", k_semantic_deps, 2},
	{"ir", "Lowering boundary before LLVM integration.", k_ir_deps, 2},
	{"driver", "Pipeline orchestration and entrypoint glue.", k_driver_deps, 4},
};

const xcompiler_module_descriptor_t *xcompiler_build_default_modules(size_t *count)
{
	if (count)
	{
		*count = sizeof(k_modules) / sizeof(k_modules[0]);
	}
	return k_modules;
}
