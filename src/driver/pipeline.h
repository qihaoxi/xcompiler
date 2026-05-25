#pragma once

#include <stddef.h>

#include "xcompiler/api/compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

const xc_stage_descriptor_t* xc_build_default_pipeline(size_t* count);

#ifdef __cplusplus
}
#endif
