#pragma once

#include "block_allocator.h"
#include "../Declarations.h"

enum json_type
{
	JSON_NULL,
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_STRING,
	JSON_INT,
	JSON_FLOAT,
	JSON_BOOL,
};

struct json_value
{
	json_value *parent;
	json_value *next_sibling;
	json_value *first_child;
	json_value *last_child;

	char *name;
	union
	{
		char *string_value;
		int   int_value;
		int64 int64_value;
		float float_value;
	};

	json_type type;
};

json_value *json_parse(char *source, char **error_pos, char **error_desc, int *error_line, block_allocator *allocator);
