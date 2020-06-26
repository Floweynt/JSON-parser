// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#ifndef __CFP_JSON_OBJECT_INTERNAL_H__
#define __CFP_JSON_OBJECT_INTERNAL_H__

#include "macros.h"

#include <map>
#include <vector>
#include <string>
#include <stdexcept>

// serves as the 'base' of the json parser. includes essensial classes and types

namespace json
{
	class json_error : public std::logic_error // errors relating to types in json
	{
	public:
		json_error(const char* p) : logic_error(p) {};
	};

	class json_type_error : public json_error // errors relating to types in json
	{
	public:
		json_type_error(const char* p) : json_error(p) {};
	};

	enum types // json types
	{
		VALUE_OBJ,
		VALUE_ARRAY,
		VALUE_STRING,
		VALUE_INT,
		VALUE_DOUBLE,
		VALUE_BOOL,
		VALUE_NULL,
	};

	namespace intern // internal class for json object
	{
		// typedefs
		class jsonobj;
		typedef std::vector<jsonobj> json_array;
		typedef std::map<std::string, jsonobj> json_obj;

		class jsonobj
		{ 
			void* to_cxx_object;
			types t;
		public:
			jsonobj();
			jsonobj(const jsonobj& rhs);
			jsonobj(types type);
			jsonobj(std::string& s);
			jsonobj(int i);
			jsonobj(double d);

			void insert_v(jsonobj j, std::string key);

			inline types get_type() { return t; }
			void set_type(types _t);

			std::map<std::string, jsonobj>& get_value_obj();
			std::string& get_value_string();
			std::vector<jsonobj>& get_value_array();
			int& get_value_int();
			double& get_value_double();
			bool& get_value_bool();
			void clear();

			jsonobj operator= (const jsonobj& rhs);
			~jsonobj();
		};
		int convert_numeric(std::string s, jsonobj& j);
	}
}
#endif // __CFP_JSON_TREE_INTERNAL_H__
