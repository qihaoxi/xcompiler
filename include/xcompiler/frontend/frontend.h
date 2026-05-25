#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xc_source_range {
	size_t offset;
	size_t length;
} xc_source_range_t;

typedef struct xc_text_view {
	const char* data;
	size_t      length;
} xc_text_view_t;

typedef enum xc_ast_node_kind {
	XC_AST_PROGRAM = 0,
	XC_AST_DECLARATION,
	XC_AST_STATEMENT,
	XC_AST_EXPRESSION,
} xc_ast_node_kind_t;

typedef struct xc_ast_node {
	xc_ast_node_kind_t kind;
	xc_source_range_t  range;
	const char*        label;
} xc_ast_node_t;

typedef struct xc_ast {
	const xc_ast_node_t* nodes;
	size_t               node_count;
} xc_ast_t;

const char* xc_ast_node_kind_to_string(xc_ast_node_kind_t kind);
void        xc_ast_init(xc_ast_t* ast);
bool        xc_ast_is_ready(const xc_ast_t* ast);

typedef enum xc_token_kind {
	XC_TOKEN_EOF = 0,
	XC_TOKEN_IDENTIFIER,
	XC_TOKEN_KEYWORD,
	XC_TOKEN_NUMBER,
	XC_TOKEN_STRING,
	XC_TOKEN_REGEX,
	XC_TOKEN_COMMENT,
	XC_TOKEN_SYMBOL,
	XC_TOKEN_UNKNOWN,
} xc_token_kind_t;

typedef struct xc_token {
	xc_token_kind_t   kind;
	xc_source_range_t range;
	const char*       lexeme;
	size_t            lexeme_length;
} xc_token_t;

typedef struct xc_lexer {
	xc_text_view_t source;
	size_t         cursor;
} xc_lexer_t;

const char*      xc_token_kind_to_string(xc_token_kind_t kind);
void             xc_lexer_init(xc_lexer_t* lexer, const char* source);
void             xc_lexer_reset(xc_lexer_t* lexer);
bool             xc_lexer_is_ready(const xc_lexer_t* lexer);
xc_status_code_t xc_lexer_next_token(xc_lexer_t* lexer, xc_token_t* out_token);

typedef struct xc_parser {
	const xc_lexer_t* lexer;
} xc_parser_t;

void             xc_parser_init(xc_parser_t* parser, const xc_lexer_t* lexer);
void             xc_parser_reset(xc_parser_t* parser);
bool             xc_parser_is_ready(const xc_parser_t* parser);
xc_status_code_t xc_parser_parse(const xc_parser_t* parser, xc_ast_t* out_ast);

typedef struct xc_semantic_context {
	const xc_ast_t* ast;
} xc_semantic_context_t;

void             xc_semantic_context_init(xc_semantic_context_t* context, const xc_ast_t* ast);
void             xc_semantic_context_reset(xc_semantic_context_t* context);
bool             xc_semantic_context_is_ready(const xc_semantic_context_t* context);
xc_status_code_t xc_semantic_analyze(const xc_semantic_context_t* context);

#ifdef __cplusplus
}
#endif
