#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "xcompiler/common/status.h"
#include "xcompiler/frontend/common.h"
#ifdef __cplusplus
extern "C" {

#endif
typedef enum xcompiler_token_kind
{
	XCOMPILER_TOKEN_EOF = 0,
	XCOMPILER_TOKEN_IDENTIFIER,
	XCOMPILER_TOKEN_NUMBER,
	XCOMPILER_TOKEN_STRING,
	XCOMPILER_TOKEN_SYMBOL,
} xcompiler_token_kind_t;

typedef struct xcompiler_token
{
	xcompiler_token_kind_t kind;
	xcompiler_source_range_t range;
} xcompiler_token_t;

typedef struct xcompiler_lexer
{
	xcompiler_text_view_t source;
	size_t cursor;
} xcompiler_lexer_t;

void xcompiler_lexer_init(xcompiler_lexer_t *lexer, const char *source);
void xcompiler_lexer_reset(xcompiler_lexer_t *lexer);
bool xcompiler_lexer_is_ready(const xcompiler_lexer_t *lexer);
xcompiler_status_code_t xcompiler_lexer_next_token(xcompiler_lexer_t *lexer, xcompiler_token_t *out_token);
const char *xcompiler_token_kind_to_string(xcompiler_token_kind_t kind);
#ifdef __cplusplus
}
#endif
