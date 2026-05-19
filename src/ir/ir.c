#include "xcompiler/ir/ir.h"

void xcompiler_ir_module_init(xcompiler_ir_module_t *module, const char *name, const xcompiler_ast_t *ast)
{
	if (!module)
	{
		return;
	}
	module->name = name;
	module->ast = ast;
}

void xcompiler_ir_module_reset(xcompiler_ir_module_t *module)
{
	if (!module)
	{
		return;
	}
	module->name = NULL;
	module->ast = NULL;
}

bool xcompiler_ir_module_is_ready(const xcompiler_ir_module_t *module)
{
	return module && module->name != NULL && module->ast != NULL;
}

xcompiler_status_code_t xcompiler_ir_module_lower(const xcompiler_ir_module_t *module)
{
	if (!xcompiler_ir_module_is_ready(module))
	{
		return XCOMPILER_STATUS_INVALID_ARGUMENT;
	}
	return XCOMPILER_STATUS_NOT_READY;
}
