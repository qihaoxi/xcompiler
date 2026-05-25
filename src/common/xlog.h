#pragma once

#include <errno.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#include <direct.h>
#define XC_XLOG_MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define XC_XLOG_MKDIR(path) mkdir(path, 0755)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum xlog_level {
	XLOG_LEVEL_TRACE   = 0,
	XLOG_LEVEL_DEBUG   = 1,
	XLOG_LEVEL_INFO    = 2,
	XLOG_LEVEL_WARNING = 3,
	XLOG_LEVEL_ERROR   = 4,
	XLOG_LEVEL_FATAL   = 5,
	XLOG_LEVEL_OFF     = 6,
} xlog_level_t;

typedef struct xc_xlog_state {
	bool         initialized;
	bool         to_console;
	FILE*        file;
	xlog_level_t level;
} xc_xlog_state_t;

extern xc_xlog_state_t xc_xlog_state;

static inline const char* xlog_sanitize_component(const char* value, const char* fallback) {
	return value && value[0] ? value : fallback;
}

static inline bool xlog_make_directory_if_needed(const char* directory) {
	if (directory[0] == '\0' || strcmp(directory, ".") == 0) {
		return true;
	}
	if (XC_XLOG_MKDIR(directory) == 0) {
		return true;
	}
	return errno == EEXIST;
}

static inline bool xlog_open_file(const char* directory, const char* name) {
	char        path[1024];
	const char* dir  = xlog_sanitize_component(directory, ".");
	const char* base = xlog_sanitize_component(name, "xcompiler");
	if (!xlog_make_directory_if_needed(dir)) {
		return false;
	}
	if (snprintf(path, sizeof(path), "%s/%s.log", dir, base) >= (int)sizeof(path)) {
		return false;
	}
	xc_xlog_state.file = fopen(path, "a");
	return xc_xlog_state.file != NULL;
}

static inline const char* xlog_level_to_string(xlog_level_t level) {
	switch (level) {
		case XLOG_LEVEL_TRACE:
			return "TRACE";
		case XLOG_LEVEL_DEBUG:
			return "DEBUG";
		case XLOG_LEVEL_INFO:
			return "INFO";
		case XLOG_LEVEL_WARNING:
			return "WARN";
		case XLOG_LEVEL_ERROR:
			return "ERROR";
		case XLOG_LEVEL_FATAL:
			return "FATAL";
		case XLOG_LEVEL_OFF:
			return "OFF";
		default:
			return "UNKNOWN";
	}
}

static inline void xlog_write_prefix(FILE* stream, xlog_level_t level, const char* file, unsigned line,
                                     const char* func) {
	time_t    now = time(NULL);
	struct tm tm_now;
	char      ts[32];
#if defined(_WIN32)
	localtime_s(&tm_now, &now);
#else
	{
		struct tm* tmp = localtime(&now);
		if (tmp) {
			tm_now = *tmp;
		} else {
			memset(&tm_now, 0, sizeof(tm_now));
		}
	}
#endif
	strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", &tm_now);
	fprintf(stream, "[%s] [%s] %s:%u %s: ", ts, xlog_level_to_string(level), file ? file : "?", line,
	        func ? func : "?");
}

static inline void xlog_shutdown(void) {
	if (xc_xlog_state.file) {
		fclose(xc_xlog_state.file);
		xc_xlog_state.file = NULL;
	}
	xc_xlog_state.initialized = false;
	xc_xlog_state.to_console  = true;
	xc_xlog_state.level       = XLOG_LEVEL_INFO;
}

static inline bool xlog_init_console(xlog_level_t level) {
	xlog_shutdown();
	xc_xlog_state.initialized = true;
	xc_xlog_state.to_console  = true;
	xc_xlog_state.level       = level;
	return true;
}

static inline bool xlog_init_file(const char* directory, const char* name, xlog_level_t level) {
	xlog_shutdown();
	if (!xlog_open_file(directory, name)) {
		return false;
	}
	xc_xlog_state.initialized = true;
	xc_xlog_state.to_console  = false;
	xc_xlog_state.level       = level;
	return true;
}

static inline bool xlog_init_full(const char* directory, const char* name, xlog_level_t level) {
	xlog_shutdown();
	if (!xlog_open_file(directory, name)) {
		return false;
	}
	xc_xlog_state.initialized = true;
	xc_xlog_state.to_console  = true;
	xc_xlog_state.level       = level;
	return true;
}

static inline bool xlog_init_daemon(const char* directory, const char* name, xlog_level_t level) {
	return xlog_init_file(directory, name, level);
}

static inline bool xlog_is_initialized(void) {
	return xc_xlog_state.initialized;
}

static inline void xlog_flush(void) {
	if (xc_xlog_state.file) {
		fflush(xc_xlog_state.file);
	}
	if (xc_xlog_state.to_console) {
		fflush(stderr);
	}
}

static inline void xlog_set_level(xlog_level_t level) {
	xc_xlog_state.level = level;
}

static inline xlog_level_t xlog_get_level(void) {
	return xc_xlog_state.level;
}

static inline bool xlog_level_enabled(xlog_level_t level) {
	return xc_xlog_state.initialized && xc_xlog_state.level != XLOG_LEVEL_OFF && level >= xc_xlog_state.level;
}

static inline void xlog_log_v(xlog_level_t level, const char* file, unsigned line, const char* func, const char* fmt,
                              va_list args) {
	if (!xlog_level_enabled(level) || !fmt) {
		return;
	}
	va_list console_args;
	va_list file_args;
	va_copy(console_args, args);
	va_copy(file_args, args);
	if (xc_xlog_state.to_console) {
		xlog_write_prefix(stderr, level, file, line, func);
		vfprintf(stderr, fmt, console_args);
		fputc('\n', stderr);
	}
	if (xc_xlog_state.file) {
		xlog_write_prefix(xc_xlog_state.file, level, file, line, func);
		vfprintf(xc_xlog_state.file, fmt, file_args);
		fputc('\n', xc_xlog_state.file);
	}
	va_end(file_args);
	va_end(console_args);
}

static inline void xlog_log(xlog_level_t level, const char* file, unsigned line, const char* func, const char* fmt,
                            ...) {
	va_list args;
	va_start(args, fmt);
	xlog_log_v(level, file, line, func, fmt, args);
	va_end(args);
}

#define XLOG_TRACE(fmt, ...) xlog_log(XLOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_DEBUG(fmt, ...) xlog_log(XLOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_INFO(fmt, ...) xlog_log(XLOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_WARN(fmt, ...) xlog_log(XLOG_LEVEL_WARNING, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_ERROR(fmt, ...) xlog_log(XLOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define XLOG_FATAL(fmt, ...) xlog_log(XLOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#ifdef XC_XLOG_IMPLEMENTATION
xc_xlog_state_t xc_xlog_state = { false, true, NULL, XLOG_LEVEL_INFO };
#endif

#ifdef __cplusplus
}
#endif
