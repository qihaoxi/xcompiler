#include "common/diagnostic.h"
#include <stdlib.h>
#include <string.h>

static char *xcompiler_strdup(const char *text)
{
	if (!text)
	{
		return NULL;
	}
	size_t length = strlen(text) + 1;
	char *copy = (char*)malloc(length);
	if (!copy)
	{
		return NULL;
	}
	memcpy(copy, text, length);
	return copy;
}

bool xcompiler_diagnostic_bag_init(xcompiler_diagnostic_bag_t *bag)
{
	if (!bag)
	{
		return false;
	}
	bag->items = NULL;
	bag->count = 0;
	bag->capacity = 0;
	return true;
}

void xcompiler_diagnostic_bag_dispose(xcompiler_diagnostic_bag_t *bag)
{
	if (!bag)
	{
		return;
	}
	for (size_t i = 0; i < bag->count; ++i)
	{
		free(bag->items[i].message);
	}
	free(bag->items);
	bag->items = NULL;
	bag->count = 0;
	bag->capacity = 0;
}

static bool xcompiler_diagnostic_bag_reserve(xcompiler_diagnostic_bag_t *bag, size_t next_capacity)
{
	if (next_capacity <= bag->capacity)
	{
		return true;
	}
	size_t capacity = bag->capacity == 0 ? 4 : bag->capacity;
	while (capacity < next_capacity)
	{
		capacity *= 2;
	}
	xcompiler_diagnostic_t *items = (xcompiler_diagnostic_t*)realloc(bag->items, capacity * sizeof(*items));
	if (!items)
	{
		return false;
	}
	bag->items = items;
	bag->capacity = capacity;
	return true;
}

bool xcompiler_diagnostic_bag_add(xcompiler_diagnostic_bag_t *bag,
								xcompiler_diagnostic_level_t level,
								const char *message)
{
	if (!bag || !message)
	{
		return false;
	}
	if (!xcompiler_diagnostic_bag_reserve(bag, bag->count + 1))
	{
		return false;
	}
	char *copy = xcompiler_strdup(message);
	if (!copy)
	{
		return false;
	}
	bag->items[bag->count].level = level;
	bag->items[bag->count].message = copy;
	bag->count += 1;
	return true;
}

bool xcompiler_diagnostic_bag_note(xcompiler_diagnostic_bag_t *bag, const char *message)
{
	return xcompiler_diagnostic_bag_add(bag, XCOMPILER_DIAGNOSTIC_INFO, message);
}

const xcompiler_diagnostic_t *xcompiler_diagnostic_bag_data(const xcompiler_diagnostic_bag_t *bag)
{
	return bag ? bag->items : NULL;
}

size_t xcompiler_diagnostic_bag_size(const xcompiler_diagnostic_bag_t *bag)
{
	return bag ? bag->count : 0;
}
