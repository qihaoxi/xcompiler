#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "../common/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum xc_ast_node_kind {
	XC_AST_PROGRAM = 0,
	XC_AST_DECLARATION,
	XC_AST_STATEMENT,
	XC_AST_EXPRESSION,
} xc_ast_node_kind_t;

typedef struct xc_ast_node {
	xc_ast_node_kind_t kind;
	xc_source_range_t  range;
	const char*        label;
} xc_ast_node_t;

typedef struct xc_ast {
	const xc_ast_node_t* nodes;
	size_t               node_count;
} xc_ast_t;

void        xc_ast_init(xc_ast_t* ast);
bool        xc_ast_is_ready(const xc_ast_t* ast);
const char* xc_ast_node_kind_to_string(xc_ast_node_kind_t kind);

#ifdef __cplusplus
}
#endif
