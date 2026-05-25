#define XC_XLOG_IMPLEMENTATION
#include "xcompiler/common/log.h"
#include "xlog.h"

bool xc_log_init_console(xc_log_level_t level) {
	return xlog_init_console((xlog_level_t)level);
}

bool xc_log_init_file(const char* directory, const char* name, xc_log_level_t level) {
	return xlog_init_file(directory, name, (xlog_level_t)level);
}

bool xc_log_init_full(const char* directory, const char* name, xc_log_level_t level) {
	return xlog_init_full(directory, name, (xlog_level_t)level);
}

bool xc_log_init_daemon(const char* directory, const char* name, xc_log_level_t level) {
	return xlog_init_daemon(directory, name, (xlog_level_t)level);
}

void xc_log_shutdown(void) {
	xlog_shutdown();
}

bool xc_log_is_initialized(void) {
	return xlog_is_initialized();
}

void xc_log_flush(void) {
	xlog_flush();
}

void xc_log_set_level(xc_log_level_t level) {
	xlog_set_level((xlog_level_t)level);
}

xc_log_level_t xc_log_get_level(void) {
	return (xc_log_level_t)xlog_get_level();
}

bool xc_log_level_enabled(xc_log_level_t level) {
	return xlog_level_enabled((xlog_level_t)level);
}

const char* xc_log_level_to_string(xc_log_level_t level) {
	return xlog_level_to_string((xlog_level_t)level);
}

void xc_log_v(xc_log_level_t level, const char* file, unsigned line, const char* func, const char* fmt, va_list args) {
	xlog_log_v((xlog_level_t)level, file, line, func, fmt, args);
}

void xc_log(xc_log_level_t level, const char* file, unsigned line, const char* func, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	xc_log_v(level, file, line, func, fmt, args);
	va_end(args);
}
