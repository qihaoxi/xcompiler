#pragma once
#include <stdbool.h>
#include "common/status.h"
#include "../ast/ast.h"
#ifdef __cplusplus
extern "C" {

#endif
typedef struct xc_semantic_context {
	const xc_ast_t* ast;
} xc_semantic_context_t;

void             xc_semantic_context_init(xc_semantic_context_t* context, const xc_ast_t* ast);
void             xc_semantic_context_reset(xc_semantic_context_t* context);
bool             xc_semantic_context_is_ready(const xc_semantic_context_t* context);
xc_status_code_t xc_semantic_analyze(const xc_semantic_context_t* context);
#ifdef __cplusplus
}
#endif
