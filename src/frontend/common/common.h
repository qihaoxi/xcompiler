#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xc_source_range {
	size_t offset;
	size_t length;
} xc_source_range_t;

typedef struct xc_text_view {
	const char* data;
	size_t      length;
} xc_text_view_t;

#ifdef __cplusplus
}
#endif
