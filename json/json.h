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

	public:
		int deserialize(const std::string& buf, JSONobj& obj);
		int deserialize_file(const std::string& filename, JSONobj& obj);
		int serialize(std::string& buf, const JSONobj& obj, bool compact = true, size_t tab = 0);
	};
}
#endif