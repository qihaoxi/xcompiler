#include "xcompiler/frontend/semantic.h"

void xcompiler_semantic_context_init(xcompiler_semantic_context_t *context, const xcompiler_ast_t *ast)
{
	if (!context)
	{
		return;
	}
	context->ast = ast;
}

void xcompiler_semantic_context_reset(xcompiler_semantic_context_t *context)
{
	if (!context)
	{
		return;
	}
	context->ast = NULL;
}

bool xcompiler_semantic_context_is_ready(const xcompiler_semantic_context_t *context)
{
	return context && context->ast != NULL;
}

xcompiler_status_code_t xcompiler_semantic_analyze(const xcompiler_semantic_context_t *context)
{
	if (!xcompiler_semantic_context_is_ready(context))
	{
		return XCOMPILER_STATUS_INVALID_ARGUMENT;
	}
	return XCOMPILER_STATUS_NOT_READY;
}
