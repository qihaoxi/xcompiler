#pragma once
#include <stdbool.h>
#include "xcompiler/common/status.h"
#include "xcompiler/frontend/ast.h"
#include "xcompiler/frontend/lexer.h"
#ifdef __cplusplus
extern "C" {

#endif
typedef struct xcompiler_parser
{
	const xcompiler_lexer_t *lexer;
} xcompiler_parser_t;

void xcompiler_parser_init(xcompiler_parser_t *parser, const xcompiler_lexer_t *lexer);
void xcompiler_parser_reset(xcompiler_parser_t *parser);
bool xcompiler_parser_is_ready(const xcompiler_parser_t *parser);
xcompiler_status_code_t xcompiler_parser_parse(const xcompiler_parser_t *parser, xcompiler_ast_t *out_ast);
#ifdef __cplusplus
}
#endif
