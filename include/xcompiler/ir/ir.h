#pragma once
#include <stdbool.h>
#include "xcompiler/common/status.h"
#include "xcompiler/frontend/ast.h"
#ifdef __cplusplus
extern "C" {

#endif
typedef struct xcompiler_ir_module
{
	const char *name;
	const xcompiler_ast_t *ast;
} xcompiler_ir_module_t;

void xcompiler_ir_module_init(xcompiler_ir_module_t *module, const char *name, const xcompiler_ast_t *ast);
void xcompiler_ir_module_reset(xcompiler_ir_module_t *module);
bool xcompiler_ir_module_is_ready(const xcompiler_ir_module_t *module);
xcompiler_status_code_t xcompiler_ir_module_lower(const xcompiler_ir_module_t *module);
#ifdef __cplusplus
}
#endif
