#include "status_internal.h"

const char *xcompiler_status_to_string(xcompiler_status_code_t code)
{
	switch (code)
	{
	case XCOMPILER_STATUS_OK:
		return "ok";
	case XCOMPILER_STATUS_INVALID_ARGUMENT:
		return "invalid-argument";
	case XCOMPILER_STATUS_NOT_READY:
		return "not-ready";
	case XCOMPILER_STATUS_UNSUPPORTED:
		return "unsupported";
	case XCOMPILER_STATUS_INTERNAL_ERROR:
		return "internal-error";
	default:
		return "unknown";
	}
}
