#include "common/arena.h"

#include <assert.h>
#include <stdlib.h>

static void test_alloc_and_reset(void) {
    xc_arena_t arena;
    xc_arena_init(&arena, 1024);

    int* a = xc_arena_alloc(&arena, sizeof(int));
    *a     = 42;
    assert(*a == 42);

    char* s = xc_arena_alloc(&arena, 128);
    for (int i = 0; i < 128; i++) {
        s[i] = (char)i;
    }

    xc_arena_reset(&arena);
    assert(arena.offset == 0);

    // allocations should work after reset
    double* d = xc_arena_alloc(&arena, sizeof(double));
    *d        = 3.14;
    assert(*d == 3.14);

    xc_arena_destroy(&arena);
    assert(arena.memory == NULL);
}

static void test_out_of_memory(void) {
    xc_arena_t arena;
    xc_arena_init(&arena, 64);

    void* p1 = xc_arena_alloc(&arena, 50);
    assert(p1 != NULL);

    void* p2 = xc_arena_alloc(&arena, 50);
    assert(p2 == NULL);

    xc_arena_destroy(&arena);
}

static void test_zero_init(void) {
    xc_arena_t arena;
    xc_arena_init(&arena, 256);
    // no allocs, just destroy
    xc_arena_destroy(&arena);
}

static void test_null_init(void) {
    xc_arena_init(NULL, 1024); // should not crash
    xc_arena_alloc(NULL, 64);  // should return NULL
    xc_arena_reset(NULL);
    xc_arena_destroy(NULL);
}

int main(void) {
    test_alloc_and_reset();
    test_out_of_memory();
    test_zero_init();
    test_null_init();
    return 0;
}
