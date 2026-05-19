#include "xcompiler/frontend/ast.h"
const char *xcompiler_ast_node_kind_to_string(xcompiler_ast_node_kind_t kind) {
switch (kind) {
case XCOMPILER_AST_PROGRAM:
return "program";
case XCOMPILER_AST_DECLARATION:
return "declaration";
case XCOMPILER_AST_STATEMENT:
return "statement";
case XCOMPILER_AST_EXPRESSION:
return "expression";
default:
return "unknown";
}
}
void xcompiler_ast_init(xcompiler_ast_t *ast) {
if (!ast) {
return;
}
ast->nodes = NULL;
ast->node_count = 0;
}
bool xcompiler_ast_is_ready(const xcompiler_ast_t *ast) {
return ast != NULL;
}
