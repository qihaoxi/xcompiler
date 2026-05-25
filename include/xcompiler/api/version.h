#pragma once

/*
 * Source-tree fallback for IDE/static analysis.
 * The build tree generates a configured version header with the same name,
 * and CMake places that generated include directory first.
 */

#ifndef XC_VERSION_MAJOR
#define XC_VERSION_MAJOR 0
#endif
#ifndef XC_VERSION_MINOR
#define XC_VERSION_MINOR 0
#endif
#ifndef XC_VERSION_PATCH
#define XC_VERSION_PATCH 0
#endif
#ifndef XC_VERSION_STRING
#define XC_VERSION_STRING "0.0.0"
#endif
