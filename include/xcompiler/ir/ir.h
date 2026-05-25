#pragma once
#include <stdbool.h>
#include "xcompiler/frontend/frontend.h"
#ifdef __cplusplus
extern "C" {

#endif
typedef struct xc_ir_module {
	const char*     name;
	const xc_ast_t* ast;
} xc_ir_module_t;

void             xc_ir_module_init(xc_ir_module_t* module, const char* name, const xc_ast_t* ast);
void             xc_ir_module_reset(xc_ir_module_t* module);
bool             xc_ir_module_is_ready(const xc_ir_module_t* module);
xc_status_code_t xc_ir_module_lower(const xc_ir_module_t* module);
#ifdef __cplusplus
}
#endif
