#include "status.h"

const char* xc_status_to_string(xc_status_code_t code) {
	switch (code) {
		case XC_STATUS_OK:
			return "ok";
		case XC_STATUS_INVALID_ARGUMENT:
			return "invalid-argument";
		case XC_STATUS_NOT_READY:
			return "not-ready";
		case XC_STATUS_UNSUPPORTED:
			return "unsupported";
		case XC_STATUS_INTERNAL_ERROR:
			return "internal-error";
		default:
			return "unknown";
	}
}
