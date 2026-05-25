#include "xcompiler/ir/ir.h"

void xc_ir_module_init(xc_ir_module_t* module, const char* name, const xc_ast_t* ast) {
	if (!module) {
		return;
	}
	module->name = name;
	module->ast  = ast;
}

void xc_ir_module_reset(xc_ir_module_t* module) {
	if (!module) {
		return;
	}
	module->name = NULL;
	module->ast  = NULL;
}

bool xc_ir_module_is_ready(const xc_ir_module_t* module) {
	return module && module->name != NULL && module->ast != NULL;
}

xc_status_code_t xc_ir_module_lower(const xc_ir_module_t* module) {
	if (!xc_ir_module_is_ready(module)) {
		return XC_STATUS_INVALID_ARGUMENT;
	}
	return XC_STATUS_NOT_READY;
}
