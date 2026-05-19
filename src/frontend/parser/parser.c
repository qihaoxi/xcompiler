#include "xcompiler/frontend/parser.h"

void xcompiler_parser_init(xcompiler_parser_t *parser, const xcompiler_lexer_t *lexer)
{
	if (!parser)
	{
		return;
	}
	parser->lexer = lexer;
}

void xcompiler_parser_reset(xcompiler_parser_t *parser)
{
	if (!parser)
	{
		return;
	}
	parser->lexer = NULL;
}

bool xcompiler_parser_is_ready(const xcompiler_parser_t *parser)
{
	return parser && parser->lexer != NULL;
}

xcompiler_status_code_t xcompiler_parser_parse(const xcompiler_parser_t *parser, xcompiler_ast_t *out_ast)
{
	if (!parser || !out_ast || !xcompiler_parser_is_ready(parser))
	{
		return XCOMPILER_STATUS_INVALID_ARGUMENT;
	}
	xcompiler_ast_init(out_ast);
	return XCOMPILER_STATUS_NOT_READY;
}
