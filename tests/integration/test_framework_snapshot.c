#include "xcompiler/api/compiler.h"
#include <assert.h>
#include <string.h>

int main(void) {
	char   buffer[512];
	size_t written = xc_framework_describe(buffer, sizeof(buffer));
	assert(written > 0);
	assert(strstr(buffer, "xcompiler") != NULL);
	assert(strstr(buffer, "pipeline stages") != NULL);
	return 0;
}
