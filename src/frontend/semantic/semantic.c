#include "semantic.h"

void xc_semantic_context_init(xc_semantic_context_t* context, const xc_ast_t* ast) {
	if (!context) {
		return;
	}
	context->ast = ast;
}

void xc_semantic_context_reset(xc_semantic_context_t* context) {
	if (!context) {
		return;
	}
	context->ast = NULL;
}

bool xc_semantic_context_is_ready(const xc_semantic_context_t* context) {
	return context && context->ast != NULL;
}

xc_status_code_t xc_semantic_analyze(const xc_semantic_context_t* context) {
	if (!xc_semantic_context_is_ready(context) || !context->ast->nodes || context->ast->node_count == 0) {
		return XC_STATUS_INVALID_ARGUMENT;
	}

	if (context->ast->nodes[0].kind != XC_AST_PROGRAM) {
		return XC_STATUS_INTERNAL_ERROR;
	}

	const size_t source_length = context->ast->nodes[0].range.length;
	for (size_t i = 0; i < context->ast->node_count; ++i) {
		const xc_ast_node_t* node = &context->ast->nodes[i];
		if (!node->label) {
			return XC_STATUS_INTERNAL_ERROR;
		}
		if (node->range.offset > source_length || node->range.length > source_length - node->range.offset) {
			return XC_STATUS_INTERNAL_ERROR;
		}
		if (i > 0 && node->kind == XC_AST_PROGRAM) {
			return XC_STATUS_INTERNAL_ERROR;
		}
	}

	return XC_STATUS_OK;
}
