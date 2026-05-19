#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum xcompiler_log_level {
XLOG_LEVEL_TRACE = 0,
XLOG_LEVEL_DEBUG = 1,
XLOG_LEVEL_INFO = 2,
XLOG_LEVEL_WARNING = 3,
XLOG_LEVEL_ERROR = 4,
XLOG_LEVEL_FATAL = 5,
XLOG_LEVEL_OFF = 6,
} xcompiler_log_level_t;
bool xlog_init_console(xcompiler_log_level_t level);
bool xlog_init_file(const char *directory, const char *name, xcompiler_log_level_t level);
bool xlog_init_full(const char *directory, const char *name, xcompiler_log_level_t level);
bool xlog_init_daemon(const char *directory, const char *name, xcompiler_log_level_t level);
void xlog_shutdown(void);
bool xlog_is_initialized(void);
void xlog_flush(void);
void xlog_set_level(xcompiler_log_level_t level);
xcompiler_log_level_t xlog_get_level(void);
bool xlog_level_enabled(xcompiler_log_level_t level);
const char *xlog_level_to_string(xcompiler_log_level_t level);
void xlog_log_v(xcompiler_log_level_t level, const char *file, unsigned line, const char *func, const char *fmt, va_list args);
void xlog_log(xcompiler_log_level_t level, const char *file, unsigned line, const char *func, const char *fmt, ...);
#define XLOG_TRACE(fmt, ...) xlog_log(XLOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_DEBUG(fmt, ...) xlog_log(XLOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_INFO(fmt, ...) xlog_log(XLOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_WARN(fmt, ...) xlog_log(XLOG_LEVEL_WARNING, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_ERROR(fmt, ...) xlog_log(XLOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_FATAL(fmt, ...) xlog_log(XLOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#ifndef XLOG_NO_LEGACY_MACROS
#define LOG_TRACE(fmt, ...) XLOG_TRACE(fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) XLOG_DEBUG(fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) XLOG_INFO(fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) XLOG_WARN(fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) XLOG_ERROR(fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) XLOG_FATAL(fmt, ##__VA_ARGS__)
#endif
#ifdef __cplusplus
}
#endif
