#include "lexer.h"

#include "char_table.h"

#include <string.h>

static const char* const k_keywords[] = {
	"break", "case",  "class", "continue", "else", "end",     "extends", "false",  "for",   "fun",   "if",   "import",
	"let",   "micro", "neta",  "none",     "note", "package", "prefer",  "return", "super", "throw", "true", "while",
};

static const char* const k_multi_symbols[] = {
	"!??", "!.?", "..=", "...", "..", "->", "=>", "::", ":=", "|>", "<<", ">>", "&&", "||", "??", "==", "!=",
	">=",  "<=",  "=~",  "!~",  "%%", "!%", "+=", "-=", "*=", "/=", "%=", ".=", ".?", "<|", "__", "_=", ",.",
};

static size_t xc_lexer_length(const xc_lexer_t* lexer) {
	return lexer ? lexer->source.length : 0;
}

static char xc_lexer_peek(const xc_lexer_t* lexer, size_t offset) {
	if (!lexer || !lexer->source.data) {
		return '\0';
	}
	size_t index = lexer->cursor + offset;
	if (index >= lexer->source.length) {
		return '\0';
	}
	return lexer->source.data[index];
}

static bool xc_lexer_matches_at(const xc_lexer_t* lexer, size_t offset, const char* text) {
	if (!lexer || !lexer->source.data || !text) {
		return false;
	}
	size_t text_length = strlen(text);
	if (lexer->cursor + offset + text_length > lexer->source.length) {
		return false;
	}
	return memcmp(lexer->source.data + lexer->cursor + offset, text, text_length) == 0;
}

static size_t xc_lexer_consume_while(const xc_lexer_t* lexer, size_t start, bool (*predicate)(char)) {
	size_t cursor = start;
	while (cursor < xc_lexer_length(lexer) && predicate(lexer->source.data[cursor])) {
		cursor++;
	}
	return cursor;
}

static size_t xc_lexer_skip_trivia(const xc_lexer_t* lexer, size_t start) {
	size_t cursor = start;
	while (cursor < xc_lexer_length(lexer)) {
		char ch = lexer->source.data[cursor];
		if (!xc_lexer_char_is_space(ch)) {
			break;
		}
		cursor++;
	}
	return cursor;
}

static bool xc_lexer_is_keyword_slice(const xc_lexer_t* lexer, size_t start, size_t length) {
	for (size_t i = 0; i < sizeof(k_keywords) / sizeof(k_keywords[0]); ++i) {
		const char* keyword        = k_keywords[i];
		size_t      keyword_length = strlen(keyword);
		if (keyword_length == length && memcmp(lexer->source.data + start, keyword, length) == 0) {
			return true;
		}
	}
	return false;
}

static xc_status_code_t xc_lexer_emit_token(xc_lexer_t* lexer, xc_token_t* out_token, xc_token_kind_t kind,
                                            size_t start, size_t end) {
	out_token->kind          = kind;
	out_token->range.offset  = start;
	out_token->range.length  = (end > start) ? (end - start) : 0;
	out_token->lexeme        = lexer->source.data ? lexer->source.data + start : NULL;
	out_token->lexeme_length = out_token->range.length;
	lexer->cursor            = end;
	return XC_STATUS_OK;
}

static xc_status_code_t xc_lexer_emit_error_token(xc_lexer_t* lexer, xc_token_t* out_token, xc_token_kind_t kind,
                                                  size_t start, size_t end) {
	(void)xc_lexer_emit_token(lexer, out_token, kind, start, end);
	return XC_STATUS_UNSUPPORTED;
}

static size_t xc_lexer_scan_identifier(const xc_lexer_t* lexer, size_t start) {
	return xc_lexer_consume_while(lexer, start, xc_lexer_char_is_identifier_continue);
}

static size_t xc_lexer_scan_decimal_digits(const xc_lexer_t* lexer, size_t start) {
	return xc_lexer_consume_while(lexer, start, xc_lexer_char_is_digit);
}

static size_t xc_lexer_scan_hex_digits(const xc_lexer_t* lexer, size_t start) {
	return xc_lexer_consume_while(lexer, start, xc_lexer_char_is_hex_digit);
}

static size_t xc_lexer_scan_number(const xc_lexer_t* lexer, size_t start) {
	size_t cursor = start;
	char   first  = xc_lexer_peek(lexer, 0);
	char   second = xc_lexer_peek(lexer, 1);

	if (first == '.') {
		cursor++;
		cursor = xc_lexer_scan_decimal_digits(lexer, cursor);
		return cursor;
	}

	if (first == '0' && (second == 'x' || second == 'X' || second == 'b' || second == 'B' || second == 'o' ||
	                     second == 'O' || second == 'z' || second == 'Z')) {
		cursor += 2;
		if (second == 'x' || second == 'X' || second == 'z' || second == 'Z') {
			cursor = xc_lexer_scan_hex_digits(lexer, cursor);
		} else {
			cursor = xc_lexer_scan_decimal_digits(lexer, cursor);
		}
		return cursor;
	}

	cursor = xc_lexer_scan_decimal_digits(lexer, cursor);
	if (xc_lexer_peek(lexer, cursor - start) == '.' &&
	    xc_lexer_char_is_digit(xc_lexer_peek(lexer, cursor - start + 1))) {
		cursor++;
		cursor = xc_lexer_scan_decimal_digits(lexer, cursor);
	}
	if (xc_lexer_peek(lexer, cursor - start) == 'e' || xc_lexer_peek(lexer, cursor - start) == 'E') {
		size_t exponent_cursor = cursor + 1;
		if (xc_lexer_peek(lexer, exponent_cursor - start) == '+' ||
		    xc_lexer_peek(lexer, exponent_cursor - start) == '-') {
			exponent_cursor++;
		}
		size_t exponent_digits = xc_lexer_scan_decimal_digits(lexer, exponent_cursor);
		if (exponent_digits > exponent_cursor) {
			cursor = exponent_digits;
		}
	}
	return cursor;
}

static bool xc_lexer_can_start_regex(const xc_lexer_t* lexer, size_t start, size_t* out_end) {
	if (!lexer || !lexer->source.data) {
		return false;
	}
	if (start >= lexer->source.length) {
		return false;
	}

	size_t cursor       = start;
	bool   escaped      = false;
	bool   seen_content = false;
	if (lexer->source.data[cursor] == '$' && cursor + 1 < lexer->source.length &&
	    lexer->source.data[cursor + 1] == '/') {
		cursor += 2;
	} else if (lexer->source.data[cursor] == '/') {
		cursor += 1;
	} else {
		return false;
	}

	while (cursor < lexer->source.length) {
		char ch = lexer->source.data[cursor];
		if (ch == '\n' || ch == '\r') {
			return false;
		}
		if (!escaped && ch == '/') {
			if (seen_content) {
				if (out_end) {
					*out_end = cursor + 1;
				}
				return true;
			}
			return false;
		}
		if (!escaped && ch == '\\') {
			escaped = true;
		} else {
			escaped      = false;
			seen_content = true;
		}
		cursor++;
	}
	return false;
}

static size_t xc_lexer_scan_string(const xc_lexer_t* lexer, size_t start, char delimiter, bool* out_terminated) {
	size_t cursor  = start;
	bool   escaped = false;
	if (lexer->source.data[cursor] == '$') {
		cursor++;
	}
	cursor++;
	while (cursor < lexer->source.length) {
		char ch = lexer->source.data[cursor];
		if (!escaped && ch == delimiter) {
			if (out_terminated) {
				*out_terminated = true;
			}
			return cursor + 1;
		}
		if (!escaped && ch == '\\') {
			escaped = true;
		} else {
			escaped = false;
		}
		cursor++;
	}
	if (out_terminated) {
		*out_terminated = false;
	}
	return cursor;
}

static size_t xc_lexer_scan_line_comment(const xc_lexer_t* lexer, size_t start) {
	size_t cursor = start + 2;
	while (cursor < lexer->source.length) {
		char ch = lexer->source.data[cursor];
		if (ch == '\n' || ch == '\r') {
			break;
		}
		cursor++;
	}
	return cursor;
}

static size_t xc_lexer_scan_block_comment(const xc_lexer_t* lexer, size_t start, bool* out_terminated) {
	size_t cursor = start + 2;
	while (cursor + 1 < lexer->source.length) {
		if (lexer->source.data[cursor] == '*' && lexer->source.data[cursor + 1] == '/') {
			if (out_terminated) {
				*out_terminated = true;
			}
			return cursor + 2;
		}
		cursor++;
	}
	if (out_terminated) {
		*out_terminated = false;
	}
	return lexer->source.length;
}

static size_t xc_lexer_scan_symbol(const xc_lexer_t* lexer, size_t start) {
	for (size_t i = 0; i < sizeof(k_multi_symbols) / sizeof(k_multi_symbols[0]); ++i) {
		const char* symbol        = k_multi_symbols[i];
		size_t      symbol_length = strlen(symbol);
		if (start + symbol_length <= lexer->source.length &&
		    memcmp(lexer->source.data + start, symbol, symbol_length) == 0) {
			return start + symbol_length;
		}
	}
	return start + 1;
}

const char* xc_token_kind_to_string(xc_token_kind_t kind) {
	switch (kind) {
		case XC_TOKEN_EOF:
			return "eof";
		case XC_TOKEN_IDENTIFIER:
			return "identifier";
		case XC_TOKEN_KEYWORD:
			return "keyword";
		case XC_TOKEN_NUMBER:
			return "number";
		case XC_TOKEN_STRING:
			return "string";
		case XC_TOKEN_REGEX:
			return "regex";
		case XC_TOKEN_COMMENT:
			return "comment";
		case XC_TOKEN_SYMBOL:
			return "symbol";
		case XC_TOKEN_UNKNOWN:
			return "unknown";
		default:
			return "unknown";
	}
}

void xc_lexer_init(xc_lexer_t* lexer, const char* source) {
	if (!lexer) {
		return;
	}
	lexer->source.data   = source;
	lexer->source.length = source ? strlen(source) : 0;
	lexer->cursor        = 0;
}

void xc_lexer_reset(xc_lexer_t* lexer) {
	if (!lexer) {
		return;
	}
	lexer->cursor = 0;
}

bool xc_lexer_is_ready(const xc_lexer_t* lexer) {
	return lexer && lexer->source.data != NULL;
}

xc_status_code_t xc_lexer_next_token(xc_lexer_t* lexer, xc_token_t* out_token) {
	if (!lexer || !out_token || !xc_lexer_is_ready(lexer)) {
		return XC_STATUS_INVALID_ARGUMENT;
	}

	lexer->cursor = xc_lexer_skip_trivia(lexer, lexer->cursor);
	if (lexer->cursor >= lexer->source.length) {
		out_token->kind          = XC_TOKEN_EOF;
		out_token->range.offset  = lexer->cursor;
		out_token->range.length  = 0;
		out_token->lexeme        = lexer->source.data + lexer->cursor;
		out_token->lexeme_length = 0;
		return XC_STATUS_NOT_READY;
	}

	size_t start = lexer->cursor;
	char   ch    = xc_lexer_peek(lexer, 0);
	char   next  = xc_lexer_peek(lexer, 1);

	if (ch == '_' && (next == '_' || next == '=')) {
		size_t end = xc_lexer_scan_symbol(lexer, start);
		return xc_lexer_emit_token(lexer, out_token, XC_TOKEN_SYMBOL, start, end);
	}

	if (xc_lexer_char_is_identifier_start(ch)) {
		size_t          end = xc_lexer_scan_identifier(lexer, start);
		xc_token_kind_t kind =
		    xc_lexer_is_keyword_slice(lexer, start, end - start) ? XC_TOKEN_KEYWORD : XC_TOKEN_IDENTIFIER;
		return xc_lexer_emit_token(lexer, out_token, kind, start, end);
	}

	if (xc_lexer_char_is_digit(ch) || (ch == '.' && xc_lexer_char_is_digit(next))) {
		size_t end = xc_lexer_scan_number(lexer, start);
		return xc_lexer_emit_token(lexer, out_token, XC_TOKEN_NUMBER, start, end);
	}

	if (ch == '"' || (ch == '$' && next == '"')) {
		bool   terminated = false;
		size_t end        = xc_lexer_scan_string(lexer, start, '"', &terminated);
		if (terminated) {
			return xc_lexer_emit_token(lexer, out_token, XC_TOKEN_STRING, start, end);
		}
		return xc_lexer_emit_error_token(lexer, out_token, XC_TOKEN_STRING, start, end);
	}

	if (ch == '/' && next == '/') {
		size_t end = xc_lexer_scan_line_comment(lexer, start);
		return xc_lexer_emit_token(lexer, out_token, XC_TOKEN_COMMENT, start, end);
	}

	if (ch == '/' && next == '*') {
		bool   terminated = false;
		size_t end        = xc_lexer_scan_block_comment(lexer, start, &terminated);
		if (terminated) {
			return xc_lexer_emit_token(lexer, out_token, XC_TOKEN_COMMENT, start, end);
		}
		return xc_lexer_emit_error_token(lexer, out_token, XC_TOKEN_COMMENT, start, end);
	}

	if (ch == '$' && next == '/') {
		size_t regex_end = 0;
		if (xc_lexer_can_start_regex(lexer, start, &regex_end)) {
			return xc_lexer_emit_token(lexer, out_token, XC_TOKEN_REGEX, start, regex_end);
		}
	}

	if (ch == '/') {
		size_t regex_end = 0;
		if (xc_lexer_can_start_regex(lexer, start, &regex_end)) {
			return xc_lexer_emit_token(lexer, out_token, XC_TOKEN_REGEX, start, regex_end);
		}
	}

	if (xc_lexer_char_is_symbol_char(ch)) {
		size_t end = xc_lexer_scan_symbol(lexer, start);
		return xc_lexer_emit_token(lexer, out_token, XC_TOKEN_SYMBOL, start, end);
	}

	return xc_lexer_emit_error_token(lexer, out_token, XC_TOKEN_UNKNOWN, start, start + 1);
}
