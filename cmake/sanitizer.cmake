# ============================================================================
# Sanitizer Configuration - Platform-Isolated
# ============================================================================
#
#
# 平台支持矩阵:
#   ┌──────────┬───────────┬────────────┬─────────────────────────────────┐
#   │ Platform │ ASan      │ TSan       │ Fiber ASan Integration          │
#   ├──────────┼───────────┼────────────┼─────────────────────────────────┤
#   │ Linux    │ ✓ Full    │ ✓ Full     │ ✓ __sanitizer_*_switch_fiber    │
#   │ macOS    │ ✗ Blocked │ ✓ Limited  │ ✗ interceptor crashes on fibers │
#   │ Windows  │ ✗ Blocked │ ✗ N/A      │ ✗ MSVC no fiber API             │
#   └──────────┴───────────┴────────────┴─────────────────────────────────┘
#
# 关键决策:
#   1. Linux: ASan/TSan 完全启用，continuation 层通过 PEL_ASAN_FIBER_SUPPORT
#      宏条件调用 fiber API
#   2. macOS: ASan 自动禁止（interceptor 与自定义栈不兼容），TSan 可用
#   4. Windows: 所有 sanitizer 自动禁止
# ============================================================================

option(SAN_ADDRESS "Enable Address Sanitizer (Linux only)" OFF)
option(SAN_THREAD "Enable Thread Sanitizer (Linux/macOS)" OFF)
option(SAN_UB "Enable Undefined Behavior Sanitizer" OFF)

# Internal flag: tracks whether any sanitizer is actually active
set(SANITIZER_ACTIVE OFF)

# Sanitizers: ASan and TSan are mutually exclusive; UBSan can combine with ASan.
if (SAN_ADDRESS AND SAN_THREAD)
    message(WARNING "ASan and TSan are mutually exclusive. Disabling TSan.")
    set(SAN_THREAD OFF)
endif ()

if (SAN_THREAD AND SAN_UB)
    message(WARNING "TSan and UBSan may interact; disabling UBSan for TSan run.")
    set(SAN_UB OFF)
endif ()

# ---- AddressSanitizer ----
if (SAN_ADDRESS)
    if (WIN32)
        message(WARNING
            "=== ASan disabled on Windows ===\n"
            "MSVC does not support ASan fiber API (__sanitizer_start_switch_fiber).\n"
            "Use Linux for ASan testing.\n"
            "SAN_ADDRESS forced OFF.")
        set(SAN_ADDRESS OFF)

    elseif (APPLE)
        message(WARNING
            "=== ASan disabled on macOS ===\n"
            "macOS ASan interceptors (memchr/printf) crash on custom fiber stacks.\n"
            "Use Linux for ASan testing.\n"
            "SAN_ADDRESS forced OFF.")
        set(SAN_ADDRESS OFF)

    else()
        # Linux / Unix: ASan fully supported
        if (CMAKE_C_COMPILER_ID STREQUAL "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_LESS "9.0")
            message(WARNING
                "ASan enabled with GCC ${CMAKE_C_COMPILER_VERSION}. "
                "GCC < 9 lacks __sanitizer_start_switch_fiber (ASan fiber API). "
                "Fiber code will use function-level no_sanitize annotations as fallback. "
                "Recommend upgrading to GCC 9+ for full fiber ASan support.")
        endif()

        message(STATUS "=== ASan enabled (Linux) ===")

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize-address-use-after-scope")
        add_link_options("-fsanitize=address" "-fsanitize-address-use-after-scope")
        # ASan + UBSan can combine (Phase 5.2: sanitizer normalization)
        if (SAN_UB)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined -fno-sanitize-recover=undefined")
            add_link_options("-fsanitize=undefined")
            message(STATUS "  UBSan: combined with ASan (-fsanitize=undefined)")
            set(SAN_UB OFF)  # already handled
        endif ()
        set(SANITIZER_ACTIVE ON)
    endif()
endif ()

# ---- ThreadSanitizer ----
if (SAN_THREAD)
    if (WIN32)
        message(WARNING
            "=== TSan disabled on Windows ===\n"
            "MSVC does not support ThreadSanitizer.\n"
            "SAN_THREAD forced OFF.")
        set(SAN_THREAD OFF)
    else()
        # Linux and macOS: TSan supported
        message(STATUS "=== TSan enabled ===")
        add_compile_options("-fsanitize=thread")
        add_link_options("-fsanitize=thread")
        set(SANITIZER_ACTIVE ON)
    endif()
endif ()

# ---- UndefinedBehaviorSanitizer ----
if (SAN_UB)
    message(STATUS "=== UBSan enabled ===")
    add_compile_options("-fsanitize=undefined" "-fno-sanitize-recover=undefined")
    add_link_options("-fsanitize=undefined")
    set(SANITIZER_ACTIVE ON)
endif ()

# ---- Expose sanitizer state to C code ----
if (SANITIZER_ACTIVE)
    add_compile_definitions(SANITIZER_ENABLED=1)
endif ()