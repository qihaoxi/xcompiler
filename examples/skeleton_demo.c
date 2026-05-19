#include "xcompiler/api/compiler.h"
#include <stdio.h>

int main(void)
{
	char buffer[512];
	xcompiler_framework_describe(buffer, sizeof(buffer));
	puts(buffer);
	return 0;
}
