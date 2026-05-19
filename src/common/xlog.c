#include "../../include/xcompiler/common/xlog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#if defined(_WIN32)
#include <direct.h>
#define XCOMPILER_XLOG_MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define XCOMPILER_XLOG_MKDIR(path) mkdir(path, 0755)
#endif
typedef struct xcompiler_xlog_state
{
	bool initialized;
	bool to_console;
	FILE *file;
	xcompiler_log_level_t level;
} xcompiler_xlog_state_t;

static xcompiler_xlog_state_t g_xlog = {false, true, NULL, XLOG_LEVEL_INFO};

static const char *xlog_sanitize_component(const char *value, const char *fallback)
{
	return value && value[0] ? value : fallback;
}

static bool xlog_make_directory_if_needed(const char *directory)
{
	if (!directory || directory[0] == '\0' || strcmp(directory, ".") == 0)
	{
		return true;
	}
	if (XCOMPILER_XLOG_MKDIR(directory) == 0)
	{
		return true;
	}
	return errno == EEXIST;
}

static bool xlog_open_file(const char *directory, const char *name)
{
	char path[1024];
	const char *dir = xlog_sanitize_component(directory, ".");
	const char *base = xlog_sanitize_component(name, "xcompiler");
	if (!xlog_make_directory_if_needed(dir))
	{
		return false;
	}
	if (snprintf(path, sizeof(path), "%s/%s.log", dir, base) >= (int)sizeof(path))
	{
		return false;
	}
	g_xlog.file = fopen(path, "a");
	return g_xlog.file != NULL;
}

static void xlog_write_prefix(FILE *stream, xcompiler_log_level_t level, const char *file, unsigned line,
							const char *func)
{
	time_t now = time(NULL);
	struct tm tm_now;
	char ts[32];
#if defined(_WIN32)
	localtime_s(&tm_now, &now);
#else
	{
		struct tm *tmp = localtime(&now);
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

bool xlog_init_console(xcompiler_log_level_t level)
{
	xlog_shutdown();
	g_xlog.initialized = true;
	g_xlog.to_console = true;
	g_xlog.level = level;
	return true;
}

bool xlog_init_file(const char *directory, const char *name, xcompiler_log_level_t level)
{
	xlog_shutdown();
	if (!xlog_open_file(directory, name))
	{
		return false;
	}
	g_xlog.initialized = true;
	g_xlog.to_console = false;
	g_xlog.level = level;
	return true;
}

bool xlog_init_full(const char *directory, const char *name, xcompiler_log_level_t level)
{
	xlog_shutdown();
	if (!xlog_open_file(directory, name))
	{
		return false;
	}
	g_xlog.initialized = true;
	g_xlog.to_console = true;
	g_xlog.level = level;
	return true;
}

bool xlog_init_daemon(const char *directory, const char *name, xcompiler_log_level_t level)
{
	return xlog_init_file(directory, name, level);
}

void xlog_shutdown(void)
{
	if (g_xlog.file)
	{
		fclose(g_xlog.file);
		g_xlog.file = NULL;
	}
	g_xlog.initialized = false;
	g_xlog.to_console = true;
	g_xlog.level = XLOG_LEVEL_INFO;
}

bool xlog_is_initialized(void)
{
	return g_xlog.initialized;
}

void xlog_flush(void)
{
	if (g_xlog.file)
	{
		fflush(g_xlog.file);
	}
	if (g_xlog.to_console)
	{
		fflush(stderr);
	}
}

void xlog_set_level(xcompiler_log_level_t level)
{
	g_xlog.level = level;
}

xcompiler_log_level_t xlog_get_level(void)
{
	return g_xlog.level;
}

bool xlog_level_enabled(xcompiler_log_level_t level)
{
	return g_xlog.initialized && g_xlog.level != XLOG_LEVEL_OFF && level >= g_xlog.level;
}

const char *xlog_level_to_string(xcompiler_log_level_t level)
{
	switch (level)
	{
	case XLOG_LEVEL_TRACE: return "TRACE";
	case XLOG_LEVEL_DEBUG: return "DEBUG";
	case XLOG_LEVEL_INFO: return "INFO";
	case XLOG_LEVEL_WARNING: return "WARN";
	case XLOG_LEVEL_ERROR: return "ERROR";
	case XLOG_LEVEL_FATAL: return "FATAL";
	case XLOG_LEVEL_OFF: return "OFF";
	default: return "UNKNOWN";
	}
}

void xlog_log_v(xcompiler_log_level_t level, const char *file, unsigned line, const char *func, const char *fmt,
				va_list args)
{
	if (!xlog_level_enabled(level) || !fmt)
	{
		return;
	}
	va_list console_args;
	va_list file_args;
	va_copy(console_args, args);
	va_copy(file_args, args);
	if (g_xlog.to_console)
	{
		xlog_write_prefix(stderr, level, file, line, func);
		vfprintf(stderr, fmt, console_args);
		fputc('\n', stderr);
	}
	if (g_xlog.file)
	{
		xlog_write_prefix(g_xlog.file, level, file, line, func);
		vfprintf(g_xlog.file, fmt, file_args);
		fputc('\n', g_xlog.file);
	}
	va_end(file_args);
	va_end(console_args);
}

void xlog_log(xcompiler_log_level_t level, const char *file, unsigned line, const char *func, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	xlog_log_v(level, file, line, func, fmt, args);
	va_end(args);
}
