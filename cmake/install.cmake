install(TARGETS xcompiler xcompiler-cli
    EXPORT xcompilerTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if (TARGET xcompiler_shared)
    install(TARGETS xcompiler_shared
        EXPORT xcompilerTargets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif ()

install(DIRECTORY ${XCOMPILER_INCLUDE_ROOT}/ DESTINATION include)
install(DIRECTORY ${XCOMPILER_GENERATED_INCLUDE_ROOT}/ DESTINATION include)

install(EXPORT xcompilerTargets
    FILE xcompilerTargets.cmake
    NAMESPACE xcompiler::
    DESTINATION lib/cmake/xcompiler
)

