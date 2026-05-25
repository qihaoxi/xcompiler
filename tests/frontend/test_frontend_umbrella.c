#include "xcompiler/frontend/frontend.h"
#include <assert.h>
#include <string.h>

int main(void) {
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_EOF), "eof") == 0);
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_IDENTIFIER), "identifier") == 0);
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_KEYWORD), "keyword") == 0);
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_NUMBER), "number") == 0);
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_STRING), "string") == 0);
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_REGEX), "regex") == 0);
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_COMMENT), "comment") == 0);
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_SYMBOL), "symbol") == 0);
	assert(strcmp(xc_token_kind_to_string(XC_TOKEN_UNKNOWN), "unknown") == 0);
	assert(strcmp(xc_ast_node_kind_to_string(XC_AST_PROGRAM), "program") == 0);
	assert(strcmp(xc_ast_node_kind_to_string(XC_AST_DECLARATION), "declaration") == 0);
	assert(strcmp(xc_ast_node_kind_to_string(XC_AST_STATEMENT), "statement") == 0);
	assert(strcmp(xc_ast_node_kind_to_string(XC_AST_EXPRESSION), "expression") == 0);
	assert(strcmp(xc_token_kind_to_string((xc_token_kind_t)999), "unknown") == 0);
	assert(strcmp(xc_ast_node_kind_to_string((xc_ast_node_kind_t)999), "unknown") == 0);
	return 0;
}
