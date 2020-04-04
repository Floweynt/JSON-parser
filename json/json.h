// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#ifndef  __CFP_JSON_PARSER_H__
#define __CFP_JSON_PARSER_H__
#include "json_obj/json_obj.h"

namespace json
{
	// error codes
	enum error_code
	{
		ERR_NO_BASE_OBJ = 1,
		ERR_UNEXPECTED_CHAR,
		ERR_BRACKETS_MISMATCH
	};
	class JSONparser // parser object
	{
		enum states // parser states
		{
			OBJ,

			WAIT_FOR_KEY,
			WAIT_FOR_KEY_OBJ,
			WAIT_FOR_COLON,
			WAIT_FOR_VALUE,
			WAIT_FOR_COMMA,

			KEY,

			STRING_VALUE,
			NUMERIC_VALUE,
			KEYWORD_VALUE,

			ARRAY_WAIT_FOR_VALUE,
			ARRAY_WAIT_FOR_COMMA,

			ARRAY_STRING_VALUE,
			ARRAY_NUMERIC_VALUE,
			ARRAY_KEYWORD_VALUE
		};

	public:
		int deserialize(std::string buf, JSONobj& obj);
		int deserialize_file(std::string filename, JSONobj& obj);
	};
}
#endif