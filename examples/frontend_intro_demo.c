#include "xcompiler/api/compiler.h"
#include "xcompiler/frontend/frontend.h"
#include "xcompiler/ir/ir.h"

#include "platform/platform.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static const char* status_to_string(xc_status_code_t status) {
	switch (status) {
		case XC_STATUS_OK:
			return "ok";
		case XC_STATUS_INVALID_ARGUMENT:
			return "invalid-argument";
		case XC_STATUS_NOT_READY:
			return "not-ready";
		case XC_STATUS_UNSUPPORTED:
			return "unsupported";
		case XC_STATUS_INTERNAL_ERROR:
			return "internal-error";
		default:
			return "unknown";
	}
}

static void print_token(const xc_token_t* token) {
	printf("  %-9s [%zu,%zu): %.*s\n", xc_token_kind_to_string(token->kind), token->range.offset,
	       token->range.offset + token->range.length, (int)token->lexeme_length, token->lexeme);
}

static void print_ast(const xc_ast_t* ast) {
	printf("AST nodes (%zu):\n", ast->node_count);
	for (size_t i = 0; i < ast->node_count; ++i) {
		const xc_ast_node_t* node = &ast->nodes[i];
		printf("  %-12s [%zu,%zu): %s\n", xc_ast_node_kind_to_string(node->kind), node->range.offset,
		       node->range.offset + node->range.length, node->label);
	}
}

int main(void) {
	const xc_framework_snapshot_t* snapshot = xc_framework_snapshot();
	assert(snapshot != NULL);

	const char* source = "let answer = 42;\n"
	                     "// a small front-end tour\n"
	                     "fun main() {\n"
	                     "  let message = \"hello\";\n"
	                     "  let pattern = $/ab\\/c/;\n"
	                     "  return answer;\n"
	                     "}\n";

	xc_lexer_t lexer;
	xc_lexer_init(&lexer, source);
	assert(xc_lexer_is_ready(&lexer));

	printf("xcompiler frontend intro\n");
	printf("version: %s\n", xc_version());
	printf("platform/backend: %s / %s\n", xc_host_platform_family(), xc_preferred_backend());
	printf("modules: %zu, pipeline stages: %zu\n", snapshot->module_count, snapshot->pipeline_count);
	printf("overview: %s -> %s\n", snapshot->modules[0].name, snapshot->pipeline[0].name);

	printf("token stream:\n");
	for (;;) {
		xc_token_t       token;
		xc_status_code_t status = xc_lexer_next_token(&lexer, &token);
		assert(status == XC_STATUS_OK || status == XC_STATUS_NOT_READY);
		print_token(&token);
		if (token.kind == XC_TOKEN_EOF) {
			break;
		}
	}

	xc_lexer_reset(&lexer);
	xc_token_t rewind_token;
	assert(xc_lexer_next_token(&lexer, &rewind_token) == XC_STATUS_OK);
	assert(rewind_token.kind == XC_TOKEN_KEYWORD);
	assert(memcmp(rewind_token.lexeme, "let", rewind_token.lexeme_length) == 0);

	xc_lexer_t parse_lexer;
	xc_lexer_init(&parse_lexer, source);

	xc_parser_t parser;
	xc_parser_init(&parser, &parse_lexer);
	assert(xc_parser_is_ready(&parser));

	xc_ast_t ast;
	xc_ast_init(&ast);
	assert(xc_ast_is_ready(&ast));
	xc_status_code_t parser_status = xc_parser_parse(&parser, &ast);
	printf("parser status: %s\n", status_to_string(parser_status));
	assert(parser_status == XC_STATUS_OK);
	print_ast(&ast);

	xc_semantic_context_t semantic;
	xc_semantic_context_init(&semantic, &ast);
	assert(xc_semantic_context_is_ready(&semantic));
	xc_status_code_t semantic_status = xc_semantic_analyze(&semantic);
	printf("semantic status: %s\n", status_to_string(semantic_status));
	assert(semantic_status == XC_STATUS_OK);

	xc_ir_module_t ir;
	xc_ir_module_init(&ir, "intro", &ast);
	assert(xc_ir_module_is_ready(&ir));
	printf("ir status: %s\n", status_to_string(xc_ir_module_lower(&ir)));

	char   summary[256];
	size_t written = xc_framework_describe(summary, sizeof(summary));
	assert(written > 0);
	puts(summary);

	return 0;
}
