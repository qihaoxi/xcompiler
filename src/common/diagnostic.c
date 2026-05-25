#include "diagnostic.h"
#include <stdlib.h>
#include <string.h>

static char* xc_strdup(const char* text) {
	if (!text) {
		return NULL;
	}
	size_t length = strlen(text) + 1;
	char*  copy   = (char*)malloc(length);
	if (!copy) {
		return NULL;
	}
	memcpy(copy, text, length);
	return copy;
}

bool xc_diagnostic_bag_init(xc_diagnostic_bag_t* bag) {
	if (!bag) {
		return false;
	}
	bag->items    = NULL;
	bag->count    = 0;
	bag->capacity = 0;
	return true;
}

void xc_diagnostic_bag_dispose(xc_diagnostic_bag_t* bag) {
	if (!bag) {
		return;
	}
	for (size_t i = 0; i < bag->count; ++i) {
		free(bag->items[i].message);
	}
	free(bag->items);
	bag->items    = NULL;
	bag->count    = 0;
	bag->capacity = 0;
}

static bool xc_diagnostic_bag_reserve(xc_diagnostic_bag_t* bag, size_t next_capacity) {
	if (next_capacity <= bag->capacity) {
		return true;
	}
	size_t capacity = bag->capacity == 0 ? 4 : bag->capacity;
	while (capacity < next_capacity) {
		capacity *= 2;
	}
	xc_diagnostic_t* items = (xc_diagnostic_t*)realloc(bag->items, capacity * sizeof(*items));
	if (!items) {
		return false;
	}
	bag->items    = items;
	bag->capacity = capacity;
	return true;
}

bool xc_diagnostic_bag_add(xc_diagnostic_bag_t* bag, xc_diagnostic_level_t level, const char* message) {
	if (!bag || !message) {
		return false;
	}
	if (!xc_diagnostic_bag_reserve(bag, bag->count + 1)) {
		return false;
	}
	char* copy = xc_strdup(message);
	if (!copy) {
		return false;
	}
	bag->items[bag->count].level   = level;
	bag->items[bag->count].message = copy;
	bag->count += 1;
	return true;
}

bool xc_diagnostic_bag_note(xc_diagnostic_bag_t* bag, const char* message) {
	return xc_diagnostic_bag_add(bag, XC_DIAGNOSTIC_LEVEL_INFO, message);
}

const xc_diagnostic_t* xc_diagnostic_bag_data(const xc_diagnostic_bag_t* bag) {
	return bag ? bag->items : NULL;
}

size_t xc_diagnostic_bag_size(const xc_diagnostic_bag_t* bag) {
	return bag ? bag->count : 0;
}
