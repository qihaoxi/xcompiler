include_guard(GLOBAL)

set(XCOMPILER_RUNTIME_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(XCOMPILER_LIBRARY_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")
set(XCOMPILER_ARCHIVE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")

function(xcompiler_apply_target_defaults target_name)
    target_compile_features(${target_name} PUBLIC c_std_11)

    if (MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    else ()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif ()

    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${XCOMPILER_RUNTIME_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${XCOMPILER_LIBRARY_OUTPUT_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${XCOMPILER_ARCHIVE_OUTPUT_DIR}"
    )
endfunction()

