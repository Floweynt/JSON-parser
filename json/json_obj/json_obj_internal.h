// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#ifndef __CFP_JSON_OBJECT_INTERNAL_H__
#define __CFP_JSON_OBJECT_INTERNAL_H__
#include <map>
#include <vector>
#include <string>

// serves as the 'base' of the json parser. includes essensial classes and types

namespace CFP
{
	namespace json
	{
		class json_type_error : public std::logic_error // errors relating to types in json
		{
		public:
			json_type_error(const char* p) : logic_error(p) {};
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
			class jsonobj
			{
				std::map<std::string, jsonobj> value_obj;
				std::vector<jsonobj> value_array;
				std::string value_string;
				int value_int;
				double value_double;
				bool value_bool;

				types t;
			public:
				jsonobj();
				jsonobj(types type);
				jsonobj(std::string s);

				void insert_v(jsonobj j, std::string key);

				inline types& get_type() { return t; }

				std::map<std::string, jsonobj>& get_value_obj();
				std::string& get_value_string();
				std::vector<jsonobj>& get_value_array();
				int& get_value_int();
				double& get_value_double();
				bool& get_value_bool();
				void clear();
			};
			int convert_numeric(std::string s, jsonobj& j);
		}
	}
}
#endif // __CFP_JSON_TREE_INTERNAL_H__
