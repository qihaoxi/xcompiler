cmake_minimum_required(VERSION 3.18)

if (DEFINED XCOMPILER_PROJECT_ROOT)
	get_filename_component(XCOMPILER_PROJECT_ROOT "${XCOMPILER_PROJECT_ROOT}" ABSOLUTE)
else ()
	set(XCOMPILER_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}")
endif ()

project(xcompiler VERSION 0.1.0 LANGUAGES C)

set(XCOMPILER_SOURCE_ROOT "${XCOMPILER_PROJECT_ROOT}/src")
set(XCOMPILER_INCLUDE_ROOT "${XCOMPILER_PROJECT_ROOT}/include")
set(XCOMPILER_GENERATED_INCLUDE_ROOT "${CMAKE_BINARY_DIR}/generated/include")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(XCOMPILER_BUILD_SHARED "Build shared xcompiler library" ON)
option(XCOMPILER_BUILD_EXAMPLES "Build xcompiler examples" ON)
option(XCOMPILER_BUILD_TESTS "Build xcompiler tests" ON)

include(${XCOMPILER_PROJECT_ROOT}/cmake/compiler-options.cmake)

configure_file(
		${XCOMPILER_INCLUDE_ROOT}/xcompiler/api/version.h.in
		${XCOMPILER_GENERATED_INCLUDE_ROOT}/xcompiler/api/version.h
		@ONLY
)

file(GLOB_RECURSE XCOMPILER_LIBRARY_SOURCES CONFIGURE_DEPENDS
		${XCOMPILER_SOURCE_ROOT}/*.c
)
list(FILTER XCOMPILER_LIBRARY_SOURCES EXCLUDE REGEX "/main\.c$")

add_library(xcompiler STATIC ${XCOMPILER_LIBRARY_SOURCES})
add_library(xcompiler::xcompiler ALIAS xcompiler)
target_include_directories(xcompiler
		PUBLIC
		$<BUILD_INTERFACE:${XCOMPILER_INCLUDE_ROOT}>
		$<BUILD_INTERFACE:${XCOMPILER_GENERATED_INCLUDE_ROOT}>
		$<BUILD_INTERFACE:${XCOMPILER_SOURCE_ROOT}>
		$<INSTALL_INTERFACE:include>
)
xcompiler_apply_target_defaults(xcompiler)

if (XCOMPILER_BUILD_SHARED)
	add_library(xcompiler_shared SHARED ${XCOMPILER_LIBRARY_SOURCES})
	set_target_properties(xcompiler_shared PROPERTIES OUTPUT_NAME xcompiler)
	target_include_directories(xcompiler_shared
			PUBLIC
			$<BUILD_INTERFACE:${XCOMPILER_INCLUDE_ROOT}>
			$<BUILD_INTERFACE:${XCOMPILER_GENERATED_INCLUDE_ROOT}>
			$<BUILD_INTERFACE:${XCOMPILER_SOURCE_ROOT}>
			$<INSTALL_INTERFACE:include>
	)
	xcompiler_apply_target_defaults(xcompiler_shared)
endif ()

add_executable(xcompiler-cli ${XCOMPILER_SOURCE_ROOT}/main.c)
target_link_libraries(xcompiler-cli PRIVATE xcompiler)
xcompiler_apply_target_defaults(xcompiler-cli)

include(${XCOMPILER_PROJECT_ROOT}/cmake/examples.cmake)

if (XCOMPILER_BUILD_TESTS)
	include(CTest)
	include(${XCOMPILER_PROJECT_ROOT}/cmake/tests.cmake)
endif ()

include(${XCOMPILER_PROJECT_ROOT}/cmake/install.cmake)

message(STATUS "xcompiler version: ${PROJECT_VERSION}")
message(STATUS "xcompiler source root: ${XCOMPILER_SOURCE_ROOT}")

