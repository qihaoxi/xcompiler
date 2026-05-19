#pragma once

#include <stddef.h>

#include "xcompiler/api/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

const xcompiler_module_descriptor_t *xcompiler_build_default_modules(size_t *count);

#ifdef __cplusplus
}
#endif
