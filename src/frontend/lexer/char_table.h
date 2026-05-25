#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool xc_lexer_char_is_space(char ch);
bool xc_lexer_char_is_newline(char ch);
bool xc_lexer_char_is_alpha(char ch);
bool xc_lexer_char_is_digit(char ch);
bool xc_lexer_char_is_hex_digit(char ch);
bool xc_lexer_char_is_identifier_start(char ch);
bool xc_lexer_char_is_identifier_continue(char ch);
bool xc_lexer_char_is_symbol_char(char ch);

#ifdef __cplusplus
}
#endif
