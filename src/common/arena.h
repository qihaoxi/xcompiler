#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct xc_arena {
	char*  memory;
	size_t capacity;
	size_t offset;
} xc_arena_t;

void  xc_arena_init(xc_arena_t* arena, size_t capacity);
void* xc_arena_alloc(xc_arena_t* arena, size_t size);
void  xc_arena_reset(xc_arena_t* arena);
void  xc_arena_destroy(xc_arena_t* arena);

#ifdef __cplusplus
}
#endif
