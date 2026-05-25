#include "xcompiler/frontend/frontend.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

static void expect_text_token(const xc_token_t* token, xc_token_kind_t kind, const char* text) {
	assert(token->kind == kind);
	assert(token->lexeme != NULL);
	assert(token->lexeme_length == strlen(text));
	assert(memcmp(token->lexeme, text, token->lexeme_length) == 0);
}

static void expect_error_token(const xc_token_t* token, xc_token_kind_t kind, const char* text) {
	expect_text_token(token, kind, text);
	assert(token->range.length == strlen(text));
}

int main(void) {
	assert(!xc_lexer_is_ready(NULL));
	xc_lexer_init(NULL, "ignored");
	xc_lexer_reset(NULL);
	xc_lexer_t lexer;
	xc_lexer_init(&lexer, NULL);
	assert(!xc_lexer_is_ready(&lexer));
	xc_token_t token;
	assert(xc_lexer_next_token(NULL, &token) == XC_STATUS_INVALID_ARGUMENT);
	assert(xc_lexer_next_token(&lexer, NULL) == XC_STATUS_INVALID_ARGUMENT);
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_INVALID_ARGUMENT);
	xc_lexer_init(&lexer, "\"unterminated string");
	assert(xc_lexer_is_ready(&lexer));
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_UNSUPPORTED);
	expect_error_token(&token, XC_TOKEN_STRING, "\"unterminated string");
	assert(token.range.offset == 0);
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_NOT_READY);
	assert(token.kind == XC_TOKEN_EOF);
	xc_lexer_init(&lexer, "/* unterminated block comment");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_UNSUPPORTED);
	expect_error_token(&token, XC_TOKEN_COMMENT, "/* unterminated block comment");
	assert(token.range.offset == 0);
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_NOT_READY);
	assert(token.kind == XC_TOKEN_EOF);
	xc_lexer_init(&lexer, "$/");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_SYMBOL, "$");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_SYMBOL, "/");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_NOT_READY);
	assert(token.kind == XC_TOKEN_EOF);
	xc_lexer_init(&lexer, "/\n");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_SYMBOL, "/");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_NOT_READY);
	assert(token.kind == XC_TOKEN_EOF);
	xc_lexer_init(&lexer, "0zFF .25 7e+8 9e 3.");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_NUMBER, "0zFF");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_NUMBER, ".25");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_NUMBER, "7e+8");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_NUMBER, "9");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_IDENTIFIER, "e");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_NUMBER, "3");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_OK);
	expect_text_token(&token, XC_TOKEN_SYMBOL, ".");
	assert(xc_lexer_next_token(&lexer, &token) == XC_STATUS_NOT_READY);
	assert(token.kind == XC_TOKEN_EOF);
	return 0;
}
