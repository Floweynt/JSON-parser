// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "json_obj.h"
#include "macros.h"
#include "../parser/parser.h"

namespace json
{
	const nulljson null{};
	std::ostream& operator<<(std::ostream& os, const JSONobj& out)
	{
		if (out.get_internal().get_type() == types::VALUE_OBJ)
		{
			std::string buf;
			intern::dumps(buf, out.get_internal(), true, 0);
			os << buf;
		}
		else if (out.get_internal().get_type() == types::VALUE_ARRAY)
		{
			std::string buf;
			intern::dumps(buf, out.get_internal(), true, 0);
			os << buf;
		}
		else if (out.get_internal().get_type() == types::VALUE_BOOL)
		{
			std::ios_base::fmtflags f = os.flags();
			os << std::boolalpha << out.get_internal().get_value_bool();
			os.flags(f);
		}
		else if (out.get_internal().get_type() == types::VALUE_DOUBLE)
			os << out.get_internal().get_value_double();
		else if (out.get_internal().get_type() == types::VALUE_INT)
			os << out.get_internal().get_value_int();
		else if (out.get_internal().get_type() == types::VALUE_NULL)
			os << "null";
		else if (out.get_internal().get_type() == types::VALUE_STRING)
			os << out.get_internal().get_value_string();
		return os;
	}
}