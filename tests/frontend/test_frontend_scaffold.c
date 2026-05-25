#include "xcompiler/frontend/frontend.h"
#include "xcompiler/ir/ir.h"
#include <assert.h>
#include <string.h>

int main(void) {
	xc_lexer_t lexer;
	xc_lexer_init(&lexer, "let answer = 42;");
	assert(xc_lexer_is_ready(&lexer));
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_IDENTIFIER), "identifier") == 0);
	xc_parser_t parser;
	xc_parser_init(&parser, &lexer);
	assert(xc_parser_is_ready(&parser));
	xc_ast_t ast;
	xc_ast_init(&ast);
	assert(xc_ast_is_ready(&ast));
	xc_semantic_context_t semantic;
	xc_semantic_context_init(&semantic, &ast);
	assert(xc_semantic_context_is_ready(&semantic));
	xc_ir_module_t ir;
	xc_ir_module_init(&ir, "demo", &ast);
	assert(xc_ir_module_is_ready(&ir));
	return 0;
}
