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
		return os;
	}
}