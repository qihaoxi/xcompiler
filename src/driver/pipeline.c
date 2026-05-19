#include "driver/pipeline.h"
static const char *const k_lexing_modules[] = {"frontend/common", "frontend/lexer"};
static const char *const k_parsing_modules[] = {"frontend/common", "frontend/lexer", "frontend/ast", "frontend/parser"};
static const char *const k_semantic_modules[] = {"frontend/semantic"};
static const char *const k_ir_modules[] = {"ir"};
static const char *const k_driver_modules[] = {"driver", "api"};
static const xcompiler_stage_descriptor_t k_pipeline[] = {
	{"lexing", "Convert source text into a token stream boundary.", k_lexing_modules, 2},
	{"parsing", "Build the AST and keep syntax ownership isolated.", k_parsing_modules, 4},
	{"semantic-analysis", "Attach symbols, scopes, and basic type checks.", k_semantic_modules, 1},
	{"ir-lowering", "Lower validated frontend state into an IR boundary.", k_ir_modules, 1},
	{"driver-orchestration", "Coordinate CLI, embedding API, and pipeline composition.", k_driver_modules, 2},
};

const xcompiler_stage_descriptor_t *xcompiler_build_default_pipeline(size_t *count)
{
	if (count)
	{
		*count = sizeof(k_pipeline) / sizeof(k_pipeline[0]);
	}
	return k_pipeline;
}
