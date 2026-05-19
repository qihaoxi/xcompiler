#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {

#endif
typedef struct xcompiler_source_range
{
	size_t offset;
	size_t length;
} xcompiler_source_range_t;

typedef struct xcompiler_text_view
{
	const char *data;
	size_t length;
} xcompiler_text_view_t;
#ifdef __cplusplus
}
#endif
