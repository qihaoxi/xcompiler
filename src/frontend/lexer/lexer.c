#include "xcompiler/frontend/lexer.h"

#include <string.h>

const char *xcompiler_token_kind_to_string(xcompiler_token_kind_t kind)
{
	switch (kind)
	{
	case XCOMPILER_TOKEN_EOF:
		return "eof";
	case XCOMPILER_TOKEN_IDENTIFIER:
		return "identifier";
	case XCOMPILER_TOKEN_NUMBER:
		return "number";
	case XCOMPILER_TOKEN_STRING:
		return "string";
	case XCOMPILER_TOKEN_SYMBOL:
		return "symbol";
	default:
		return "unknown";
	}
}

void xcompiler_lexer_init(xcompiler_lexer_t *lexer, const char *source)
{
	if (!lexer)
	{
		return;
	}
	lexer->source.data = source;
	lexer->source.length = source ? strlen(source) : 0;
	lexer->cursor = 0;
}

void xcompiler_lexer_reset(xcompiler_lexer_t *lexer)
{
	if (!lexer)
	{
		return;
	}
	lexer->cursor = 0;
}

bool xcompiler_lexer_is_ready(const xcompiler_lexer_t *lexer)
{
	return lexer && lexer->source.data != NULL;
}

xcompiler_status_code_t xcompiler_lexer_next_token(xcompiler_lexer_t *lexer, xcompiler_token_t *out_token)
{
	if (!lexer || !out_token || !xcompiler_lexer_is_ready(lexer))
	{
		return XCOMPILER_STATUS_INVALID_ARGUMENT;
	}
	out_token->kind = XCOMPILER_TOKEN_EOF;
	out_token->range.offset = lexer->cursor;
	out_token->range.length = 0;
	return XCOMPILER_STATUS_NOT_READY;
}
