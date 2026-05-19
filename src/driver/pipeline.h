#pragma once

#include <stddef.h>

#include "../../include/xcompiler/api/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

const xcompiler_stage_descriptor_t *xcompiler_build_default_pipeline(size_t *count);

#ifdef __cplusplus
}
#endif
