#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "common/status.h"
#include "../common/common.h"
#ifdef __cplusplus
extern "C" {

#endif
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

void             xc_lexer_init(xc_lexer_t* lexer, const char* source);
void             xc_lexer_reset(xc_lexer_t* lexer);
bool             xc_lexer_is_ready(const xc_lexer_t* lexer);
xc_status_code_t xc_lexer_next_token(xc_lexer_t* lexer, xc_token_t* out_token);
const char*      xc_token_kind_to_string(xc_token_kind_t kind);
#ifdef __cplusplus
}
#endif
