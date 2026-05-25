#include "arena.h"

#include <stdalign.h>
#include <stdint.h>
#include <string.h>

#ifdef XC_USE_MIMALLOC
#include <mimalloc.h>
#define xc_raw_alloc(size) mi_malloc(size)
#define xc_raw_free(ptr) mi_free(ptr)
#else
#include <stdlib.h>
#define xc_raw_alloc(size) malloc(size)
#define xc_raw_free(ptr) free(ptr)
#endif

void xc_arena_init(xc_arena_t* arena, size_t capacity) {
	if (!arena) {
		return;
	}
	arena->memory   = capacity > 0 ? (char*)xc_raw_alloc(capacity) : NULL;
	arena->capacity = arena->memory ? capacity : 0;
	arena->offset   = 0;
}

static size_t align_up(size_t size, size_t alignment) {
	return (size + alignment - 1) & ~(alignment - 1);
}

void* xc_arena_alloc(xc_arena_t* arena, size_t size) {
	if (!arena || !arena->memory || size == 0) {
		return NULL;
	}
	size_t aligned = align_up(arena->offset, alignof(max_align_t));
	if (aligned + size > arena->capacity) {
		return NULL;
	}
	void* ptr     = arena->memory + aligned;
	arena->offset = aligned + size;
	memset(ptr, 0, size);
	return ptr;
}

void xc_arena_reset(xc_arena_t* arena) {
	if (!arena) {
		return;
	}
	arena->offset = 0;
}

void xc_arena_destroy(xc_arena_t* arena) {
	if (!arena) {
		return;
	}
	xc_raw_free(arena->memory);
	arena->memory   = NULL;
	arena->capacity = 0;
	arena->offset   = 0;
}
