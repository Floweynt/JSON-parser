// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#ifndef __CFP_JSON_PARSER_H__
#define __CFP_JSON_PARSER_H__

#include "json_obj/json_obj.h"
#include "json_obj/macros.h"
#include "parser/parser.h"

namespace json
{
	class JSON // parser object
	{

	public:
		int deserialize(const std::string& buf, JSONobj& obj);
		int deserialize_file(const std::string& filename, JSONobj& obj);
		int deserialize_stream(std::istream& stream, JSONobj& obj);
		int serialize(std::string& buf, const JSONobj& obj, size_t tab = 0);
		int serialize_file(const std::string& filename, const JSONobj& obj, size_t tab = 0);
		int serialize_stream(std::ostream& stream, const JSONobj& obj, size_t tab = 0);

	};
}
#endif