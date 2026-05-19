if (NOT XCOMPILER_BUILD_EXAMPLES)
	return()
endif ()

set(XCOMPILER_EXAMPLE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/examples")
file(MAKE_DIRECTORY "${XCOMPILER_EXAMPLE_OUTPUT_DIR}")

function(add_xcompiler_example target_name source_file)
	add_executable(${target_name} ${source_file})
	target_link_libraries(${target_name} PRIVATE xcompiler)
	xcompiler_apply_target_defaults(${target_name})
	set_target_properties(${target_name} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY "${XCOMPILER_EXAMPLE_OUTPUT_DIR}"
	)
endfunction()

add_xcompiler_example(xcompiler_skeleton_demo
		${XCOMPILER_PROJECT_ROOT}/examples/skeleton_demo.c
)

add_xcompiler_example(xcompiler_logging_demo
		${XCOMPILER_PROJECT_ROOT}/examples/logging_demo.c
)

message(STATUS "xcompiler examples enabled")

