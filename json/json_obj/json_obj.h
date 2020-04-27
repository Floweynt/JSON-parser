// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "json_obj_internal.h"
#include <ostream>
#include <stdexcept>

#ifdef _WIN32
#pragma warning(disable: 4996) 
#pragma warning(disable: 26812)
#endif

namespace json
{
	struct nulljson {};
	extern const nulljson null;

	class JSONobj
	{
		intern::jsonobj object;

	public:
		inline JSONobj() { }
		JSONobj(intern::jsonobj o) { this->object = o; }
		inline operator std::string() { return object.get_value_string(); }
		inline bool operator!=(std::string& rhs) { return object.get_value_string() != rhs; }
		inline bool operator!=(int rhs) { return object.get_value_int() != rhs; }
		inline bool operator!=(double rhs) { return object.get_value_double() != rhs; }
		inline bool operator!=(const char* rhs) { return object.get_value_string() != rhs; }
		inline bool operator!=(bool rhs) { return object.get_value_bool() != rhs; }
		inline bool operator!=(nulljson rhs) { return object.get_type() == types::VALUE_NULL; }
		inline intern::jsonobj get_internal() const { return this->object; }

		JSONobj operator [](const std::string index) { return this->object.get_value_obj()[index]; }
		JSONobj operator [](const int index) { return this->object.get_value_array()[index]; }

		friend std::ostream& operator<<(std::ostream& os, const JSONobj& out);
	};
}