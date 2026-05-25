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

install(FILES
    ${XCOMPILER_INCLUDE_ROOT}/xcompiler/api/compiler.h
    DESTINATION include/xcompiler/api
)
install(FILES
    ${XCOMPILER_GENERATED_INCLUDE_ROOT}/xcompiler/api/version.h
    DESTINATION include/xcompiler/api
)
install(FILES
    ${XCOMPILER_INCLUDE_ROOT}/xcompiler/common/log.h
    DESTINATION include/xcompiler/common
)
install(FILES
    ${XCOMPILER_INCLUDE_ROOT}/xcompiler/frontend/frontend.h
    DESTINATION include/xcompiler/frontend
)
install(FILES
    ${XCOMPILER_INCLUDE_ROOT}/xcompiler/ir/ir.h
    DESTINATION include/xcompiler/ir
)

install(EXPORT xcompilerTargets
    FILE xcompilerTargets.cmake
    NAMESPACE xcompiler::
    DESTINATION lib/cmake/xcompiler
)

