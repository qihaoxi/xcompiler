#pragma once
#include <stdbool.h>
#include <stddef.h>
#include "xcompiler/frontend/common.h"
#ifdef __cplusplus
extern "C" {

#endif
typedef enum xcompiler_ast_node_kind
{
	XCOMPILER_AST_PROGRAM = 0,
	XCOMPILER_AST_DECLARATION,
	XCOMPILER_AST_STATEMENT,
	XCOMPILER_AST_EXPRESSION,
} xcompiler_ast_node_kind_t;

typedef struct xcompiler_ast_node
{
	xcompiler_ast_node_kind_t kind;
	xcompiler_source_range_t range;
	const char *label;
} xcompiler_ast_node_t;

typedef struct xcompiler_ast
{
	const xcompiler_ast_node_t *nodes;
	size_t node_count;
} xcompiler_ast_t;

void xcompiler_ast_init(xcompiler_ast_t *ast);
bool xcompiler_ast_is_ready(const xcompiler_ast_t *ast);
const char *xcompiler_ast_node_kind_to_string(xcompiler_ast_node_kind_t kind);
#ifdef __cplusplus
}
#endif
