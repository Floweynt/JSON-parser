// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#ifndef __CFP_JSON_OBJ_H__
#define __CFP_JSON_OBJ_H__
#include "macros.h"

#include <unordered_map>
#include <vector>
#include <ostream>
#include <stdexcept>

namespace json
{
	struct nulljson {};
	constexpr nulljson null;

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

	class JSONobj
	{
		// should use std::variant, but C++17 bad
		void* to_value;
		types t;

	public:
		typedef JSONobj value_type;
		typedef value_type& reference;
		typedef const reference const_reference;
		typedef size_t size_type;
		 
		typedef std::vector<JSONobj> json_array;
		typedef bool json_bool;
		typedef double json_float;
		typedef int64_t json_int;
		typedef std::unordered_map<std::string, JSONobj> json_obj;
		typedef std::string json_string;

		// loads errors
		enum error
		{
			ERR_BRACKETS_MISMATCH = 1,
			ERR_NO_BASE_OBJ,
			ERR_UNEXPECTED_CHAR,
			ERR_NOT_VALID_ESCAPE,
			ERR_NOT_NUM,
		};

		// ctors
		inline JSONobj() { this->nullify(); }
		JSONobj(const JSONobj& rhs);		// initalize via copy
		JSONobj(JSONobj&& rhs) noexcept;	// initalize via move
		JSONobj(types t);					// initalize w/ type

		// custom destructor
		~JSONobj();

		// construct with STL containers / c++ types

		// consturct array
		//JSONobj(const std::vector<)

		// construct bool
		JSONobj(const json_bool init);

		// consruct double
		JSONobj(const json_float init);

		// consturct int
		JSONobj(const json_int init);

		// construct string
		JSONobj(const std::string& init);

		/******** element access ********/

		// will throw exception when done primitives, or if you use incorrect key
		inline reference at(const json_obj::key_type& key) { return this->get_object()[key]; }
		inline reference at(const json_array::size_type index) { return this->get_array()[index]; }

		inline reference operator [](const json_obj::key_type key) { return this->at(key); }
		inline reference operator [](const size_type index) { return this->at(index); }

		// on primitives, returns that, on objects/arrays, does behavior specified by the stl
		inline reference front() {}
		inline reference back() {};

		// size

		// empty attemps to get size of internal stl object, however primitives will return false
		bool empty();

		// modifiers

		void push_back(const json_obj::value_type& val);
		void push_back(const json_array::value_type& val);

		// basic get operations
		// we are not going to use template for right now

		json_obj& get_object();
		json_array& get_array();
		json_string& get_string();
		json_float& get_float();
		json_int& get_int();

		// type operations

		void nullify();

		void set_type(types t);
		inline types get_type() const { return this->t; }

		inline bool is_array() const { return this->t == types::VALUE_ARRAY; }
		inline bool is_bool() const { return this->t == types::VALUE_BOOL; }
		inline bool is_float() const { return this->t == types::VALUE_DOUBLE; }
		inline bool is_int() const { return this->t == types::VALUE_INT; }
		inline bool is_null() const { return this->t == types::VALUE_NULL; }
		inline bool is_object() const { return this->t == types::VALUE_OBJ; }
		inline bool is_string() const { return this->t == types::VALUE_STRING; }

		
		// serialize and deserailze
		
		int loads(const std::string& buf);
		int dumps(std::string& buf);
		int loads_file(const std::string& fname);
		int dumps_file(const std::string& fname);
		int loads_stream(std::istream& stream);
		int dumps_stream(std::ostream& st);

		// operators

		reference operator= (const_reference rhs);

		// compares

		/*inline bool operator!=(std::string& rhs) { return object.get_value_string() != rhs; }
		inline bool operator!=(int rhs) { return object.get_value_int() != rhs; }*
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

		// converters
		inline explicit operator std::string() { return object.get_value_string(); }
		inline explicit operator int() { return object.get_value_int(); }
		inline explicit operator double() { return object.get_value_double(); }
		inline explicit operator bool() { return object.get_value_bool(); }

		inline intern::jsonobj get_internal() const { return this->object; }

		friend std::ostream& operator<<(std::ostream& os, const JSONobj& out);*/
	};
}
#endif