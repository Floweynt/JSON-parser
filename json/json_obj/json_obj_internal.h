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
#ifdef _DEBUG
		extern int cctorc;
		extern int eoperc;
#endif

		class jsonobj
		{ 
			// typedefs 
			typedef std::vector<jsonobj> json_array;
			typedef std::map<std::string, jsonobj> json_obj;

			// DONT use std::variant, i want this to be cxx11 as much as possible
			void* to_cxx_object;
			types t;
		public:
			// C++ required ctors: 

			// default ctor
			jsonobj();

			// copy ctor: copies the type and creates new ptr
			jsonobj(const jsonobj& rhs);

			// move ctor: invalidates rhs, and sets buffer to rhs's buffer
			jsonobj(jsonobj&& rhs) noexcept;

			// initalize with a types
			jsonobj(const types type);

			// copy non-json objects
			jsonobj(const std::string& s);
			jsonobj(const int i);
			jsonobj(const double d);

			// given a string, insert a numeric value (can be a integer or double, will determine this later)
			int insert_numeric(std::string& rhs, std::string& key);

			// insert_v functions: inserts a value into current json object. This is internal, and api will be simpler

			// specializes string insert
			void insert_v(const std::string& val, const std::string& key);

			// invalidates j, and 'moves' into this
			void insert_v(jsonobj& j, const std::string& key);

			// we have to specialize type changes (need to delete other types so no memory leaks)
			inline types get_type() { return t; }
			void set_type(types _t);

			// obtain referances to internal objects
			std::map<std::string, jsonobj>& get_value_obj();
			std::string& get_value_string();
			std::vector<jsonobj>& get_value_array();
			int& get_value_int();
			double& get_value_double();
			bool& get_value_bool();
			void clear();

			// copy-assignment operator **OPTIMIZE TO NOT USE THIS!**
			jsonobj operator= (const jsonobj& rhs);

			// custom destructor
			~jsonobj();
		};
	}
}
#endif // __CFP_JSON_TREE_INTERNAL_H__
