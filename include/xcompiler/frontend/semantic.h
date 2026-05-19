#pragma once
#include <stdbool.h>
#include "xcompiler/common/status.h"
#include "xcompiler/frontend/ast.h"
#ifdef __cplusplus
extern "C" {

#endif
typedef struct xcompiler_semantic_context
{
	const xcompiler_ast_t *ast;
} xcompiler_semantic_context_t;

void xcompiler_semantic_context_init(xcompiler_semantic_context_t *context, const xcompiler_ast_t *ast);
void xcompiler_semantic_context_reset(xcompiler_semantic_context_t *context);
bool xcompiler_semantic_context_is_ready(const xcompiler_semantic_context_t *context);
xcompiler_status_code_t xcompiler_semantic_analyze(const xcompiler_semantic_context_t *context);
#ifdef __cplusplus
}
#endif
