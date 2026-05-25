#pragma once

#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {

#endif

typedef enum xc_log_level {
	XC_LOG_LEVEL_TRACE   = 0,
	XC_LOG_LEVEL_DEBUG   = 1,
	XC_LOG_LEVEL_INFO    = 2,
	XC_LOG_LEVEL_WARNING = 3,
	XC_LOG_LEVEL_ERROR   = 4,
	XC_LOG_LEVEL_FATAL   = 5,
	XC_LOG_LEVEL_OFF     = 6,
} xc_log_level_t;

bool           xc_log_init_console(xc_log_level_t level);
bool           xc_log_init_file(const char* directory, const char* name, xc_log_level_t level);
bool           xc_log_init_full(const char* directory, const char* name, xc_log_level_t level);
bool           xc_log_init_daemon(const char* directory, const char* name, xc_log_level_t level);
void           xc_log_shutdown(void);
bool           xc_log_is_initialized(void);
void           xc_log_flush(void);
void           xc_log_set_level(xc_log_level_t level);
xc_log_level_t xc_log_get_level(void);
bool           xc_log_level_enabled(xc_log_level_t level);
const char*    xc_log_level_to_string(xc_log_level_t level);
void xc_log_v(xc_log_level_t level, const char* file, unsigned line, const char* func, const char* fmt, va_list args);
void xc_log(xc_log_level_t level, const char* file, unsigned line, const char* func, const char* fmt, ...);

#define XC_LOG_TRACE(fmt, ...) xc_log(XC_LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XC_LOG_DEBUG(fmt, ...) xc_log(XC_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XC_LOG_INFO(fmt, ...) xc_log(XC_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XC_LOG_WARN(fmt, ...) xc_log(XC_LOG_LEVEL_WARNING, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XC_LOG_ERROR(fmt, ...) xc_log(XC_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XC_LOG_FATAL(fmt, ...) xc_log(XC_LOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
