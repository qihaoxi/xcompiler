#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum xcompiler_diagnostic_level {
  XCOMPILER_DIAGNOSTIC_INFO = 0,
  XCOMPILER_DIAGNOSTIC_WARNING,
  XCOMPILER_DIAGNOSTIC_ERROR,
} xcompiler_diagnostic_level_t;

typedef struct xcompiler_diagnostic {
  xcompiler_diagnostic_level_t level;
  char *message;
} xcompiler_diagnostic_t;

typedef struct xcompiler_diagnostic_bag {
  xcompiler_diagnostic_t *items;
  size_t count;
  size_t capacity;
} xcompiler_diagnostic_bag_t;

bool xcompiler_diagnostic_bag_init(xcompiler_diagnostic_bag_t *bag);
void xcompiler_diagnostic_bag_dispose(xcompiler_diagnostic_bag_t *bag);
bool xcompiler_diagnostic_bag_add(xcompiler_diagnostic_bag_t *bag,
    xcompiler_diagnostic_level_t level,
    const char *message);
bool xcompiler_diagnostic_bag_note(xcompiler_diagnostic_bag_t *bag, const char *message);
const xcompiler_diagnostic_t *xcompiler_diagnostic_bag_data(const xcompiler_diagnostic_bag_t *bag);
size_t xcompiler_diagnostic_bag_size(const xcompiler_diagnostic_bag_t *bag);

#ifdef __cplusplus
}
#endif

