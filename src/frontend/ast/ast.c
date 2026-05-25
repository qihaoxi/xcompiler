#include "ast.h"

const char* xc_ast_node_kind_to_string(xc_ast_node_kind_t kind) {
	switch (kind) {
		case XC_AST_PROGRAM:
			return "program";
		case XC_AST_DECLARATION:
			return "declaration";
		case XC_AST_STATEMENT:
			return "statement";
		case XC_AST_EXPRESSION:
			return "expression";
		default:
			return "unknown";
	}
}

void xc_ast_init(xc_ast_t* ast) {
	if (!ast) {
		return;
	}
	ast->nodes      = NULL;
	ast->node_count = 0;
}

bool xc_ast_is_ready(const xc_ast_t* ast) {
	return ast != NULL;
}
