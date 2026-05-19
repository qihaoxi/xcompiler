#include "xcompiler/frontend/ast.h"
#include "xcompiler/frontend/lexer.h"
#include "xcompiler/frontend/parser.h"
#include "xcompiler/frontend/semantic.h"
#include "xcompiler/ir/ir.h"
#include <assert.h>
#include <string.h>

int main(void)
{
	xcompiler_lexer_t lexer;
	xcompiler_lexer_init(&lexer, "let answer = 42;");
	assert(xcompiler_lexer_is_ready(&lexer));
	assert(strcmp(xcompiler_token_kind_to_string(XCOMPILER_TOKEN_IDENTIFIER), "identifier") == 0);
	xcompiler_parser_t parser;
	xcompiler_parser_init(&parser, &lexer);
	assert(xcompiler_parser_is_ready(&parser));
	xcompiler_ast_t ast;
	xcompiler_ast_init(&ast);
	assert(xcompiler_ast_is_ready(&ast));
	xcompiler_semantic_context_t semantic;
	xcompiler_semantic_context_init(&semantic, &ast);
	assert(xcompiler_semantic_context_is_ready(&semantic));
	xcompiler_ir_module_t ir;
	xcompiler_ir_module_init(&ir, "demo", &ast);
	assert(xcompiler_ir_module_is_ready(&ir));
	return 0;
}
