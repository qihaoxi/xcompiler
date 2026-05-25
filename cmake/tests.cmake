set(XCOMPILER_TEST_OUTPUT_DIR "${CMAKE_BINARY_DIR}/tests")
file(MAKE_DIRECTORY "${XCOMPILER_TEST_OUTPUT_DIR}")

enable_testing()

function(add_xcompiler_test target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE xcompiler)
    xcompiler_apply_target_defaults(${target_name})
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${XCOMPILER_TEST_OUTPUT_DIR}"
    )
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

add_xcompiler_test(test_compiler_facade
    ${XCOMPILER_PROJECT_ROOT}/tests/api/test_compiler_facade.c
)

add_xcompiler_test(test_framework_describe
	${XCOMPILER_PROJECT_ROOT}/tests/api/test_framework_describe.c
)

add_xcompiler_test(test_pipeline_layout
    ${XCOMPILER_PROJECT_ROOT}/tests/driver/test_pipeline_layout.c
)

add_xcompiler_test(test_framework_snapshot
    ${XCOMPILER_PROJECT_ROOT}/tests/integration/test_framework_snapshot.c
)

add_xcompiler_test(test_frontend_scaffold
    ${XCOMPILER_PROJECT_ROOT}/tests/frontend/test_frontend_scaffold.c
)

add_xcompiler_test(test_frontend_lexer
    ${XCOMPILER_PROJECT_ROOT}/tests/frontend/test_frontend_lexer.c
)

add_xcompiler_test(test_frontend_lexer_edges
  ${XCOMPILER_PROJECT_ROOT}/tests/frontend/test_frontend_lexer_edges.c
)

add_xcompiler_test(test_frontend_intro
	${XCOMPILER_PROJECT_ROOT}/tests/frontend/test_frontend_intro.c
)

add_xcompiler_test(test_xlog
    ${XCOMPILER_PROJECT_ROOT}/tests/common/test_xlog.c
)

add_xcompiler_test(test_log_facade
    ${XCOMPILER_PROJECT_ROOT}/tests/common/test_log_facade.c
)

add_xcompiler_test(test_diagnostic
    ${XCOMPILER_PROJECT_ROOT}/tests/common/test_diagnostic.c
)

add_xcompiler_test(test_arena
    ${XCOMPILER_PROJECT_ROOT}/tests/common/test_arena.c
)

add_xcompiler_test(test_frontend_umbrella
    ${XCOMPILER_PROJECT_ROOT}/tests/frontend/test_frontend_umbrella.c
)

add_test(NAME xcompiler_cli_version COMMAND xcompiler-cli --version)

