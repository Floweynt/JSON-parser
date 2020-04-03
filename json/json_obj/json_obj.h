// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "json_obj_internal.h"
#include <ostream>
#include <stdexcept>

#ifdef _WIN32
#pragma warning(disable: 4996) 
#pragma warning(disable: 26812)
#endif

namespace CFP
{
	namespace json
	{
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
			inline bool operator!=(const char* rhs) { return object.get_value_string() != (std::string)rhs; }

			JSONobj operator [](const std::string index) { return this->object.get_value_obj()[index]; }
			JSONobj operator [](const int index) { return this->object.get_value_array()[index]; }

			friend std::ostream& operator<<(std::ostream& os, const JSONobj& out);
			bool operator ==(JSONobj& rhs);
		};
	}
}