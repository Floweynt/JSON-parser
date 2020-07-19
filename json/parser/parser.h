// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#ifndef __CFP_JSON_PARSER_INTERNAL_H__
#define __CFP_JSON_PARSER_INTERNAL_H__

#include "../json_obj/json_obj_internal.h"
#include "../json_obj/macros.h"

#include <iostream>

namespace json
{
	enum deseralize_error
	{
		ERR_NO_BASE_OBJ = 1,
		ERR_UNEXPECTED_CHAR,
		ERR_BRACKETS_MISMATCH,
		ERR_NOT_NUM,
		ERR_NOT_VALID_ESCAPE
	};
	namespace intern
	{
		int loads(const std::string& buf, jsonobj& obj);
		int dumps(std::string& buf, const jsonobj& obj, size_t tab = 0);
	}
}

#endif