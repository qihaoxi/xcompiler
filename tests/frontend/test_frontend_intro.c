#include "xcompiler/frontend/frontend.h"
#include "xcompiler/ir/ir.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

typedef struct expected_token {
	xc_token_kind_t kind;
	const char*     text;
	size_t          offset;
} expected_token_t;

typedef struct expected_ast_node {
	xc_ast_node_kind_t kind;
	const char*        label;
	size_t             offset;
} expected_ast_node_t;

static void expect_token(const xc_token_t* token, const expected_token_t* expected) {
	assert(token->kind == expected->kind);
	assert(token->lexeme != NULL);
	assert(token->lexeme_length == strlen(expected->text));
	assert(memcmp(token->lexeme, expected->text, token->lexeme_length) == 0);
	assert(token->range.offset == expected->offset);
	assert(token->range.length == strlen(expected->text));
}

static void expect_ast_node(const xc_ast_node_t* node, const expected_ast_node_t* expected) {
	assert(node->kind == expected->kind);
	assert(node->label != NULL);
	assert(strcmp(node->label, expected->label) == 0);
	assert(node->range.offset == expected->offset);
	assert(node->range.length > 0);
}

int main(void) {
	const char* source = "let answer = 42;\n"
	                     "fun main() { return answer; }\n";

	xc_lexer_t lexer;
	xc_lexer_init(&lexer, source);
	assert(xc_lexer_is_ready(&lexer));
	assert(xc_lexer_next_token(NULL, NULL) == XC_STATUS_INVALID_ARGUMENT);
	assert(xc_lexer_next_token(&lexer, NULL) == XC_STATUS_INVALID_ARGUMENT);

	const expected_token_t expected[] = {
		{ XC_TOKEN_KEYWORD,    "let",    0  },
        { XC_TOKEN_IDENTIFIER, "answer", 4  },
		{ XC_TOKEN_SYMBOL,     "=",      11 },
        { XC_TOKEN_NUMBER,     "42",     13 },
		{ XC_TOKEN_SYMBOL,     ";",      15 },
        { XC_TOKEN_KEYWORD,    "fun",    17 },
		{ XC_TOKEN_IDENTIFIER, "main",   21 },
        { XC_TOKEN_SYMBOL,     "(",      25 },
		{ XC_TOKEN_SYMBOL,     ")",      26 },
        { XC_TOKEN_SYMBOL,     "{",      28 },
		{ XC_TOKEN_KEYWORD,    "return", 30 },
        { XC_TOKEN_IDENTIFIER, "answer", 37 },
		{ XC_TOKEN_SYMBOL,     ";",      43 },
        { XC_TOKEN_SYMBOL,     "}",      45 },
	};
	for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i) {
		xc_token_t token;
		assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
		expect_token(&token, &expected[i]);
	}

	xc_token_t eof_token;
	assert(xc_lexer_next_token(&lexer, &eof_token) == XC_STATUS_NOT_READY);
	assert(eof_token.kind == XC_TOKEN_EOF);
	assert(eof_token.range.offset == strlen(source));
	assert(eof_token.range.length == 0);
	assert(eof_token.lexeme_length == 0);

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
	assert(xc_parser_parse(NULL, NULL) == XC_STATUS_INVALID_ARGUMENT);
	assert(xc_parser_parse(&parser, NULL) == XC_STATUS_INVALID_ARGUMENT);

	xc_ast_t ast;
	xc_ast_init(&ast);
	assert(xc_ast_is_ready(&ast));
	assert(xc_parser_parse(&parser, &ast) == XC_STATUS_OK);
	assert(ast.nodes != NULL);
	assert(ast.node_count == 4);
	assert(ast.nodes[0].range.offset == 0);
	assert(ast.nodes[0].range.length == strlen(source));

	const expected_ast_node_t expected_ast[] = {
		{ XC_AST_PROGRAM,     "program", 0  },
		{ XC_AST_DECLARATION, "answer",  0  },
		{ XC_AST_DECLARATION, "main",    17 },
		{ XC_AST_STATEMENT,   "return",  30 },
	};
	for (size_t i = 0; i < sizeof(expected_ast) / sizeof(expected_ast[0]); ++i) {
		expect_ast_node(&ast.nodes[i], &expected_ast[i]);
	}

	xc_semantic_context_t semantic;
	xc_semantic_context_init(&semantic, &ast);
	assert(xc_semantic_context_is_ready(&semantic));
	assert(xc_semantic_analyze(NULL) == XC_STATUS_INVALID_ARGUMENT);
	assert(xc_semantic_analyze(&semantic) == XC_STATUS_OK);

	xc_ir_module_t ir;
	xc_ir_module_init(&ir, "intro", &ast);
	assert(xc_ir_module_is_ready(&ir));
	assert(xc_ir_module_lower(NULL) == XC_STATUS_INVALID_ARGUMENT);
	assert(xc_ir_module_lower(&ir) == XC_STATUS_NOT_READY);

	xc_parser_reset(&parser);
	assert(!xc_parser_is_ready(&parser));
	xc_semantic_context_reset(&semantic);
	assert(!xc_semantic_context_is_ready(&semantic));
	xc_ir_module_reset(&ir);
	assert(!xc_ir_module_is_ready(&ir));
	return 0;
}
