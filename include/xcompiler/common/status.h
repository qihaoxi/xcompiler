#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum xcompiler_status_code
{
	XCOMPILER_STATUS_OK = 0,
	XCOMPILER_STATUS_INVALID_ARGUMENT,
	XCOMPILER_STATUS_NOT_READY,
	XCOMPILER_STATUS_UNSUPPORTED,
	XCOMPILER_STATUS_INTERNAL_ERROR,
};

typedef enum xcompiler_status_code xcompiler_status_code_t;

const char *xcompiler_status_to_string(xcompiler_status_code_t code);

#ifdef __cplusplus
}
#endif
