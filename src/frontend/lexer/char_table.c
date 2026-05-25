#include "char_table.h"

#include <ctype.h>

bool xc_lexer_char_is_space(char ch) {
	return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r' || ch == '\n';
}

bool xc_lexer_char_is_newline(char ch) {
	return ch == '\r' || ch == '\n';
}

bool xc_lexer_char_is_alpha(char ch) {
	unsigned char uch = (unsigned char)ch;
	return (uch >= 'a' && uch <= 'z') || (uch >= 'A' && uch <= 'Z');
}

bool xc_lexer_char_is_digit(char ch) {
	unsigned char uch = (unsigned char)ch;
	return uch >= '0' && uch <= '9';
}

bool xc_lexer_char_is_hex_digit(char ch) {
	unsigned char uch = (unsigned char)ch;
	return xc_lexer_char_is_digit(ch) || (uch >= 'a' && uch <= 'f') || (uch >= 'A' && uch <= 'F');
}

bool xc_lexer_char_is_identifier_start(char ch) {
	return ch == '_' || xc_lexer_char_is_alpha(ch);
}

bool xc_lexer_char_is_identifier_continue(char ch) {
	return xc_lexer_char_is_identifier_start(ch) || xc_lexer_char_is_digit(ch);
}

bool xc_lexer_char_is_symbol_char(char ch) {
	switch (ch) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case '.':
		case ',':
		case ':':
		case ';':
		case '&':
		case '|':
		case '!':
		case '^':
		case '~':
		case '$':
		case '@':
		case '#':
		case '<':
		case '>':
		case '?':
		case '=':
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
			return true;
		default:
			return false;
	}
}
