#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum xc_diagnostic_level {
	XC_DIAGNOSTIC_LEVEL_INFO    = 0,
	XC_DIAGNOSTIC_LEVEL_WARNING = 1,
	XC_DIAGNOSTIC_LEVEL_ERROR   = 2,
} xc_diagnostic_level_t;

typedef struct xc_diagnostic {
	xc_diagnostic_level_t level;
	char*                 message;
} xc_diagnostic_t;

typedef struct xc_diagnostic_bag {
	xc_diagnostic_t* items;
	size_t           count;
	size_t           capacity;
} xc_diagnostic_bag_t;

bool xc_diagnostic_bag_init(xc_diagnostic_bag_t* bag);
void xc_diagnostic_bag_dispose(xc_diagnostic_bag_t* bag);
bool xc_diagnostic_bag_add(xc_diagnostic_bag_t* bag, xc_diagnostic_level_t level, const char* message);
bool xc_diagnostic_bag_note(xc_diagnostic_bag_t* bag, const char* message);
const xc_diagnostic_t* xc_diagnostic_bag_data(const xc_diagnostic_bag_t* bag);
size_t                 xc_diagnostic_bag_size(const xc_diagnostic_bag_t* bag);

#ifdef __cplusplus
}
#endif
