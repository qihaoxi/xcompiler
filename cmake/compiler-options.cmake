include_guard(GLOBAL)

option(ENABLE_STRICT_CHECKS "Enable stricter compiler diagnostics" ON)

set(XCOMPILER_RUNTIME_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(XCOMPILER_LIBRARY_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")
set(XCOMPILER_ARCHIVE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")

function(xcompiler_apply_target_defaults target_name)
    target_compile_features(${target_name} PUBLIC c_std_11)

    if (MSVC)
        target_compile_options(${target_name} PRIVATE
                /W4
                /WX
                /permissive-
                /utf-8
                /Zc:__cplusplus
        )

        if (ENABLE_STRICT_CHECKS)
            target_compile_options(${target_name} PRIVATE /sdl)
        endif ()
    else ()
        target_compile_options(${target_name} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wwrite-strings
                -Wcast-align
                -Wuninitialized
                -ffile-prefix-map=${XCOMPILER_PROJECT_ROOT}/=
                -fmacro-prefix-map=${XCOMPILER_PROJECT_ROOT}/=
                -fPIC # Position Independent Code for shared libraries
        )

        if (ENABLE_STRICT_CHECKS)
            target_compile_options(${target_name} PRIVATE
                    -Werror
                    -Wstrict-prototypes
                    -Wmissing-declarations
                    -Wno-sign-compare
                    -Wno-unused-parameter
                    -Wno-unused-function
            )

            if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND UNIX)
                target_compile_options(${target_name} PRIVATE
                        -fstack-protector
                        -fstack-protector-strong
                        -fstack-clash-protection
                )
            elseif (CMAKE_C_COMPILER_ID MATCHES "Clang" AND UNIX)
                target_compile_options(${target_name} PRIVATE
                        -fstack-protector-strong
                )
            endif ()
        endif ()
    endif ()

    set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${XCOMPILER_RUNTIME_OUTPUT_DIR}"
            LIBRARY_OUTPUT_DIRECTORY "${XCOMPILER_LIBRARY_OUTPUT_DIR}"
            ARCHIVE_OUTPUT_DIRECTORY "${XCOMPILER_ARCHIVE_OUTPUT_DIR}"
    )
endfunction()