#include "platform/platform.h"

#if defined(_WIN32)
static const char* k_platform_family = "windows";
#elif defined(__APPLE__)
static const char* k_platform_family = "apple";
#else
static const char* k_platform_family = "unix";
#endif

#if defined(__clang__)
static const char* k_backend = "clang/llvm";
#elif defined(__GNUC__)
static const char* k_backend = "gcc/portable-c";
#else
static const char* k_backend = "portable-c";
#endif

const char* xc_host_platform_family(void) {
	return k_platform_family;
}

const char* xc_preferred_backend(void) {
	return k_backend;
}
