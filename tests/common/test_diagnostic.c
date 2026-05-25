#include "common/diagnostic.h"
#include <assert.h>
#include <string.h>

int main(void) {
	xc_diagnostic_bag_t bag;
	assert(xc_diagnostic_bag_init(&bag));
	assert(xc_diagnostic_bag_size(&bag) == 0);
	assert(xc_diagnostic_bag_note(&bag, "note one"));
	assert(xc_diagnostic_bag_add(&bag, XC_DIAGNOSTIC_LEVEL_WARNING, "warn two"));
	assert(xc_diagnostic_bag_size(&bag) == 2);
	assert(strcmp(xc_diagnostic_bag_data(&bag)[0].message, "note one") == 0);
	assert(xc_diagnostic_bag_data(&bag)[1].level == XC_DIAGNOSTIC_LEVEL_WARNING);
	xc_diagnostic_bag_dispose(&bag);
	assert(xc_diagnostic_bag_size(&bag) == 0);
	return 0;
}
