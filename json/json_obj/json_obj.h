// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#ifndef __CFP_JSON_OBJ_H__
#define __CFP_JSON_OBJ_H__

#include "json_obj_internal.h"
#include "macros.h"

#include <ostream>
#include <stdexcept>

namespace json
{
	struct nulljson {};
	constexpr nulljson null;

	class JSONobj
	{
		intern::jsonobj object;

	public:
		inline JSONobj() { }
		inline JSONobj(intern::jsonobj& o) { object = o; }
		inline void copy_from_internal(intern::jsonobj& o) { std::swap(o, object); }
		
		inline bool operator!=(std::string& rhs) { return object.get_value_string() != rhs; }
		inline bool operator!=(int rhs) { return object.get_value_int() != rhs; }
		inline bool operator!=(double rhs) { return object.get_value_double() != rhs; }
		inline bool operator!=(const char* rhs) { return object.get_value_string() != rhs; }
		inline bool operator!=(bool rhs) { return object.get_value_bool() != rhs; }
		inline bool operator!=(nulljson rhs) { return object.get_type() == types::VALUE_NULL; }

		inline bool operator==(std::string& rhs) { return object.get_value_string() == rhs; }
		inline bool operator==(int rhs) { return object.get_value_int() == rhs; }
		inline bool operator==(double rhs) { return object.get_value_double() == rhs; }
		inline bool operator==(const char* rhs) { return object.get_value_string() == rhs; }
		inline bool operator==(bool rhs) { return object.get_value_bool() == rhs; }
		inline bool operator==(nulljson rhs) { return object.get_type() == types::VALUE_NULL; }

		inline explicit operator std::string() { return object.get_value_string(); }
		inline explicit operator int() { return object.get_value_int(); }
		inline explicit operator double() { return object.get_value_double(); }
		inline explicit operator bool() { return object.get_value_bool(); }

		inline intern::jsonobj get_internal() const { return this->object; }

		JSONobj operator [](const std::string index) { return this->object.get_value_obj()[index]; }
		JSONobj operator [](const int index) { return this->object.get_value_array()[index]; }

		friend std::ostream& operator<<(std::ostream& os, const JSONobj& out);
	};
}
#endif