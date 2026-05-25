cmake_minimum_required(VERSION 3.10)

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

option(ENABLE_SHARED "Build shared xcompiler library" ON)
option(ENABLE_EXAMPLES "Build xcompiler examples" ON)
option(ENABLE_TESTS "Build xcompiler tests" ON)
option(USE_MIMALLOC "Use mimalloc as arena backing allocator" OFF)

include(${XCOMPILER_PROJECT_ROOT}/cmake/sanitizer.cmake)
include(${XCOMPILER_PROJECT_ROOT}/cmake/debug.cmake)
include(${XCOMPILER_PROJECT_ROOT}/cmake/compiler-options.cmake)

if (USE_MIMALLOC)
    find_library(MIMALLOC_LIBRARY mimalloc)
    find_path(MIMALLOC_INCLUDE_DIR mimalloc.h)
    if (MIMALLOC_LIBRARY AND MIMALLOC_INCLUDE_DIR)
        message(STATUS "mimalloc found: ${MIMALLOC_LIBRARY}")
        set(MIMALLOC_FOUND TRUE)
    else()
        message(WARNING "USE_MIMALLOC is ON but mimalloc not found — falling back to malloc")
        set(USE_MIMALLOC OFF)
    endif()
endif()

add_custom_target(format
        COMMAND ${CMAKE_COMMAND}
        -DXCOMPILER_PROJECT_ROOT=${XCOMPILER_PROJECT_ROOT}
        -P ${XCOMPILER_PROJECT_ROOT}/cmake/format.cmake
        WORKING_DIRECTORY ${XCOMPILER_PROJECT_ROOT}
        COMMENT "Running clang-format over xcompiler sources"
)

configure_file(
        ${XCOMPILER_PROJECT_ROOT}/cmake/version.h.in
        ${XCOMPILER_GENERATED_INCLUDE_ROOT}/xcompiler/api/version.h
        @ONLY
)

file(GLOB_RECURSE XCOMPILER_LIBRARY_SOURCES CONFIGURE_DEPENDS
        ${XCOMPILER_SOURCE_ROOT}/*.c
)
file(GLOB_RECURSE XCOMPILER_LIBRARY_HEADERS CONFIGURE_DEPENDS
        ${XCOMPILER_SOURCE_ROOT}/*.h
)
list(FILTER XCOMPILER_LIBRARY_SOURCES EXCLUDE REGEX "/main\.c$")
#list(FILTER XCOMPILER_LIBRARY_SOURCES EXCLUDE REGEX "/xlog\.c$")

add_library(xcompiler STATIC ${XCOMPILER_LIBRARY_SOURCES} ${XCOMPILER_LIBRARY_HEADERS})
add_library(xcompiler::xcompiler ALIAS xcompiler)
target_include_directories(xcompiler
        PUBLIC
        $<BUILD_INTERFACE:${XCOMPILER_GENERATED_INCLUDE_ROOT}>
        $<BUILD_INTERFACE:${XCOMPILER_INCLUDE_ROOT}>
        $<BUILD_INTERFACE:${XCOMPILER_SOURCE_ROOT}>
        $<INSTALL_INTERFACE:include>
)
xcompiler_apply_target_defaults(xcompiler)
xcompiler_apply_debug_options(xcompiler)

if (ENABLE_SHARED)
    add_library(xcompiler_shared SHARED ${XCOMPILER_LIBRARY_SOURCES} ${XCOMPILER_LIBRARY_HEADERS})
    set_target_properties(xcompiler_shared PROPERTIES OUTPUT_NAME xcompiler)
    target_include_directories(xcompiler_shared
            PUBLIC
            $<BUILD_INTERFACE:${XCOMPILER_GENERATED_INCLUDE_ROOT}>
            $<BUILD_INTERFACE:${XCOMPILER_INCLUDE_ROOT}>
            $<BUILD_INTERFACE:${XCOMPILER_SOURCE_ROOT}>
            $<INSTALL_INTERFACE:include>
    )
    xcompiler_apply_target_defaults(xcompiler_shared)
    xcompiler_apply_debug_options(xcompiler_shared)
endif ()

if (USE_MIMALLOC)
    target_compile_definitions(xcompiler PUBLIC XC_USE_MIMALLOC)
    target_include_directories(xcompiler PUBLIC ${MIMALLOC_INCLUDE_DIR})
    target_link_libraries(xcompiler PUBLIC ${MIMALLOC_LIBRARY})
    if (ENABLE_SHARED)
        target_compile_definitions(xcompiler_shared PUBLIC XC_USE_MIMALLOC)
        target_include_directories(xcompiler_shared PUBLIC ${MIMALLOC_INCLUDE_DIR})
        target_link_libraries(xcompiler_shared PUBLIC ${MIMALLOC_LIBRARY})
    endif()
endif()

add_executable(xcompiler-cli ${XCOMPILER_SOURCE_ROOT}/main.c)
target_link_libraries(xcompiler-cli PRIVATE xcompiler)
xcompiler_apply_target_defaults(xcompiler-cli)
xcompiler_apply_debug_options(xcompiler-cli)

if (ENABLE_EXAMPLES)
    include(${XCOMPILER_PROJECT_ROOT}/cmake/examples.cmake)
endif ()

if (ENABLE_TESTS)
    include(CTest)
    include(${XCOMPILER_PROJECT_ROOT}/cmake/tests.cmake)
endif ()

include(${XCOMPILER_PROJECT_ROOT}/cmake/install.cmake)

message(STATUS "xcompiler version: ${PROJECT_VERSION}")
message(STATUS "xcompiler source root: ${XCOMPILER_SOURCE_ROOT}")

