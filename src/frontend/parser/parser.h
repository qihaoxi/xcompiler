#pragma once
#include <stdbool.h>
#include "common/status.h"
#include "../ast/ast.h"
#include "../lexer/lexer.h"
#ifdef __cplusplus
extern "C" {

#endif
typedef struct xc_parser {
	const xc_lexer_t* lexer;
} xc_parser_t;

void             xc_parser_init(xc_parser_t* parser, const xc_lexer_t* lexer);
void             xc_parser_reset(xc_parser_t* parser);
bool             xc_parser_is_ready(const xc_parser_t* parser);
xc_status_code_t xc_parser_parse(const xc_parser_t* parser, xc_ast_t* out_ast);
#ifdef __cplusplus
}
#endif
