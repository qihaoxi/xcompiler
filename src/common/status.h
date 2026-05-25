#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum xc_status_code {
	XC_STATUS_OK = 0,
	XC_STATUS_INVALID_ARGUMENT,
	XC_STATUS_NOT_READY,
	XC_STATUS_UNSUPPORTED,
	XC_STATUS_INTERNAL_ERROR,
};

typedef enum xc_status_code xc_status_code_t;

const char* xc_status_to_string(xc_status_code_t code);

#ifdef __cplusplus
}
#endif
