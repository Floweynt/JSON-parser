// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "json.h"
#include "macros.h"
#include <stack>

namespace json
{
	/** CTORS **/
	JSONobj::JSONobj(const JSONobj& rhs)
	{
		this->t = rhs.t;
		// switch on type for copy
		switch (rhs.t)
		{
		case json_types::VALUE_OBJ:
			this->to_value = new json_obj(*(json_obj*)rhs.to_value);
			break;
		case json_types::VALUE_ARRAY:
			this->to_value = new json_array(*(json_array*)rhs.to_value);
			break;
		case json_types::VALUE_STRING:
			this->to_value = new json_string(*(json_string*)rhs.to_value);
			break;
		case json_types::VALUE_INT:
			this->to_value = new json_int(*(json_int*)rhs.to_value);
			break;
		case json_types::VALUE_DOUBLE:
			this->to_value = new json_float(*(json_float*)rhs.to_value);
			break;
		case json_types::VALUE_BOOL:
			this->to_value = new json_bool(*(json_bool*)rhs.to_value);
			break;
		case json_types::VALUE_NULL:
			this->to_value = nullptr;
			break;
		default:
			this->to_value = nullptr;
			break;
		}
	}

	JSONobj::JSONobj(JSONobj&& rhs) noexcept
	{
		this->to_value = rhs.to_value;
		this->t = rhs.t;
		rhs.to_value = nullptr;
		rhs.t = json_types::VALUE_NULL;
	}

	JSONobj::JSONobj(json_types t)
	{
		this->t = t;
		switch (t)
		{
		case json::JSONobj::VALUE_OBJ:
			this->to_value = new json_obj;
			break;
		case json::JSONobj::VALUE_ARRAY:
			this->to_value = new json_array;
			break;
		case json::JSONobj::VALUE_STRING:
			this->to_value = new json_string;
			break;
		case json::JSONobj::VALUE_INT:
			this->to_value = new json_int(0);
			break;
		case json::JSONobj::VALUE_DOUBLE:
			this->to_value = new json_float(0);
			break;
		case json::JSONobj::VALUE_BOOL:
			this->to_value = new json_bool(false);
			break;
		case json::JSONobj::VALUE_NULL:
			this->to_value = nullptr;
			break;
		default:
			this->to_value = nullptr;
			this->t = json_types::VALUE_NULL;
			break;
		}
	}

	/** DESTRUCTOR **/
	JSONobj::~JSONobj()
	{
		// switch on type for delete
		switch (this->t)
		{
		case json_types::VALUE_OBJ:
			delete (json_obj*)this->to_value;
			break;
		case json_types::VALUE_ARRAY:
			delete (json_array*)this->to_value;
			break;
		case json_types::VALUE_STRING:
			delete (json_string*)this->to_value;
			break;
		case json_types::VALUE_INT:
			delete (json_int*)this->to_value;
			break;
		case json_types::VALUE_DOUBLE:
			delete (json_float*)this->to_value;
			break;
		case json_types::VALUE_BOOL:
			delete (json_bool*)this->to_value;
			break;
		case json_types::VALUE_NULL:
			// uhhh idk nothing to do here
			break;
		default:
			break;
		}
		this->t = json_types::VALUE_NULL;
		this->to_value = nullptr;
	}

	/** CONSTRUCT WITH STL CONTAINER OR C++ PRIMATIVE **/

	JSONobj::JSONobj(const JSONobj::json_bool init)
	{
		this->t = json_types::VALUE_BOOL;
		this->to_value = new json_bool(init);
	}

	JSONobj::JSONobj(const JSONobj::json_float init)
	{
		this->t = json_types::VALUE_DOUBLE;
		this->to_value = new json_float(init);
	}

	JSONobj::JSONobj(const JSONobj::json_int init)
	{
		this->t = json_types::VALUE_INT;
		this->to_value = new json_int(init);
	}

	JSONobj::JSONobj(const std::string& init)
	{
		this->t = json_types::VALUE_STRING;
		this->to_value = new json_string(init);
	}

	/** ELEMENT ACCESS **/

	/** MOTIFIERS **/
	void JSONobj::push_back(const JSONobj::json_obj::value_type& val)
	{
		if (this->t != json_types::VALUE_OBJ)
			throw json_type_error("unable to insert key-value pair in non-object type");
		((json_obj*)this->to_value)->insert(val);
	}

	void JSONobj::push_back(const JSONobj::json_array::value_type& val)
	{
		if (this->t != json_types::VALUE_ARRAY)
			throw json_type_error("unable to insert value in non-array type");
		((json_array*)this->to_value)->push_back(val);
	}

	/** BASIC GET OPERATIONS **/
	JSONobj::json_array& JSONobj::get_array()
	{
		if (this->t != json_types::VALUE_ARRAY)
			throw json_type_error("unable to get array in non-array type");
		return *(json_array*)this->to_value;
	}

	JSONobj::json_float& JSONobj::get_float()
	{
		if (this->t != json_types::VALUE_DOUBLE)
			throw json_type_error("unable to get float in non-float type");
		return *(json_float*)this->to_value;
	}

	JSONobj::json_int& JSONobj::get_int()
	{
		if (this->t != json_types::VALUE_INT)
			throw json_type_error("unable to get int in non-int type");
		return *(json_int*)this->to_value;
	}

	JSONobj::json_obj& JSONobj::get_object()
	{
		if (this->t != json_types::VALUE_OBJ)
			throw json_type_error("unable to get object in non-object type");
		return *(json_obj*)this->to_value;
	}

	JSONobj::json_string& JSONobj::get_string()
	{
		if (this->t != json_types::VALUE_STRING)
			throw json_type_error("unable to get string in non-string type");
		return *(json_string*)this->to_value;
	}

	/** TYPE OPERATIONS **/
	void JSONobj::nullify()
	{
		// free data
		switch (this->t)
		{
		case json_types::VALUE_OBJ:
			delete (json_obj*)this->to_value;
			break;
		case json_types::VALUE_ARRAY:
			delete (json_array*)this->to_value;
			break;
		case json_types::VALUE_STRING:
			delete (json_string*)this->to_value;
			break;
		case json_types::VALUE_INT:
			delete (json_int*)this->to_value;
			break;
		case json_types::VALUE_DOUBLE:
			delete (json_float*)this->to_value;
			break;
		case json_types::VALUE_BOOL:
			delete (json_bool*)this->to_value;
			break;
		case json_types::VALUE_NULL:
			// uhhh idk nothing to do here
			break;
		default:
			break;
		}

		// set to null
		this->t = json_types::VALUE_NULL;
		this->to_value = nullptr;
	}

	JSONobj::reference JSONobj::operator=(JSONobj::const_reference rhs)
	{
		// commit self deletus
		// so our memory doesnt leak
		
		// switch on type for delete
		switch (this->t)
		{
		case json_types::VALUE_OBJ:
			delete (json_obj*)this->to_value;
			break;
		case json_types::VALUE_ARRAY:
			delete (json_array*)this->to_value;
			break;
		case json_types::VALUE_STRING:
			delete (json_string*)this->to_value;
			break;
		case json_types::VALUE_INT:
			delete (json_int*)this->to_value;
			break;
		case json_types::VALUE_DOUBLE:
			delete (json_float*)this->to_value;
			break;
		case json_types::VALUE_BOOL:
			delete (json_bool*)this->to_value;
			break;
		case json_types::VALUE_NULL:
			// uhhh idk nothing to do here
			break;
		default:
			break;
		}

		this->t = rhs.t;
		// switch on type for copy
		switch (rhs.t)
		{
		case json_types::VALUE_OBJ:
			this->to_value = new json_obj(*(json_obj*)rhs.to_value);
			break;
		case json_types::VALUE_ARRAY:
			this->to_value = new json_array(*(json_array*)rhs.to_value);
			break;
		case json_types::VALUE_STRING:
			this->to_value = new json_string(*(json_string*)rhs.to_value);
			break;
		case json_types::VALUE_INT:
			this->to_value = new json_int(*(json_int*)rhs.to_value);
			break;
		case json_types::VALUE_DOUBLE:
			this->to_value = new json_float(*(json_float*)rhs.to_value);
			break;
		case json_types::VALUE_BOOL:
			this->to_value = new json_bool(*(json_bool*)rhs.to_value);
			break;
		case json_types::VALUE_NULL:
			this->to_value = nullptr;
			break;
		default:
			break;
		}
		return *this;
	}

	void JSONobj::set_type(json_types t)
	{
		this->nullify();
	}

	bool JSONobj::empty()
	{
		if (this->t == json_types::VALUE_ARRAY)
			return this->get_array().empty();
		else if (this->t == json_types::VALUE_OBJ)
			return this->get_object().empty();
		else
			return true;
	}

	JSONobj::size_type JSONobj::size()
	{
		if (this->t == json_types::VALUE_ARRAY)
			return this->get_array().size();
		else if (this->t == json_types::VALUE_OBJ)
			return this->get_object().size();
		else
			return -1U;
	}
	
	/*
	JSONobj::iterator JSONobj::begin()
	{
	
	}

	JSONobj::iterator JSONobj::end()
	{

	}
	*/
	int JSONobj::loads_file(const std::string& fname)
	{
		std::string buf;
		FILE* fd = fopen(buf.c_str(), "r");
		if (!fd)
			return -1;

		// obtain size of file
		fseek(fd, 0, SEEK_END);
		size_t size = ftell(fd);
		rewind(fd);

		buf.resize(size);
		fread((void*)buf.data(), 1, size, fd);
		return this->loads(buf);
	}

	int JSONobj::dumps_file(const std::string& fname)
	{
		std::string buf;
		int ret = dumps(buf);
		FILE* fd = fopen(fname.c_str(), "w");
		if (!fd)
			return -1;

		fwrite((void*)buf.data(), 1, buf.size(), fd);
		return ret;
	}

	int JSONobj::loads_stream(std::istream& stream)
	{
		std::string buf;
		stream >> buf;
		return this->loads(buf);
	}
	int JSONobj::dumps_stream(std::ostream& stream)
	{
		std::string buf;
		int ret = this->loads(buf);
		stream << buf;
		return ret;
	}

	JSONobj::iterator::iterator(const json_array::iterator& _it_array)
	{
		this->t = json_iterator_types::VALUE_ARRAY;
		this->iter = new json_array::iterator(_it_array);
	}

	JSONobj::iterator::iterator(const json_obj::iterator& _it_obj)
	{
		this->t = json_iterator_types::VALUE_OBJ;
		this->iter = new json_obj::iterator(_it_obj);
	}

	// prefix ++ operator
	JSONobj::iterator JSONobj::iterator::operator++()
	{
		switch (t)
		{
		case JSONobj::iterator::VALUE_OBJ:
			return (*(json_obj::iterator*)iter)++; // increment
		case JSONobj::iterator::VALUE_ARRAY:
			return (*(json_array::iterator*)iter)++; // increment
		default:
			break;
		}
	}

	// suffix ++ operator
	JSONobj::iterator JSONobj::iterator::operator++(int)
	{
		switch (t)
		{
		case JSONobj::iterator::VALUE_OBJ:
			return ++(*(json_obj::iterator*)iter); // increment
		case JSONobj::iterator::VALUE_ARRAY:
			return ++(*(json_array::iterator*)iter); // increment
		default:
			break;
		}
	}

	/*
	std::string JSONobj::iterator::key()
	{

	}
	

	// dereferance operator
	JSONobj::iterator::reference JSONobj::iterator::operator*()
	{

	}
	*/
	/*std::ostream& operator<<(std::ostream& os, const JSONobj& out)
	{
		if (out.get_internal().get_type() == types::VALUE_OBJ)
		{
			std::string buf;
			intern::dumps(buf, out.get_internal(), 0);
			os << buf;
		}
		else if (out.get_internal().get_type() == types::VALUE_ARRAY)
		{
			std::string buf;
			intern::dumps(buf, out.get_internal(), 0);
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
	}*/
}