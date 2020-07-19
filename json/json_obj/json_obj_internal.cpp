// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "macros.h"
#include "json_obj_internal.h"
#include <iostream>

namespace json
{
	namespace intern
	{
		jsonobj::jsonobj()
		{
			this->t = types::VALUE_NULL;
			this->to_cxx_object = nullptr;
		}

		jsonobj::jsonobj(const jsonobj& rhs)
		{
			this->t = rhs.t;

			if (rhs.to_cxx_object)
			{
				switch (this->t)
				{
				case types::VALUE_ARRAY:
					this->to_cxx_object = new json_array(*(json_array*)rhs.to_cxx_object);
					break;
				case types::VALUE_BOOL:
					this->to_cxx_object = new bool(*(bool*)rhs.to_cxx_object);
					break;
				case types::VALUE_DOUBLE:
					this->to_cxx_object = new double(*(double*)rhs.to_cxx_object);
					break;
				case types::VALUE_INT:
					this->to_cxx_object = new int(*(int*)rhs.to_cxx_object);
					break;
				case types::VALUE_OBJ:
					this->to_cxx_object = new json_obj(*(json_obj*)rhs.to_cxx_object);
					break;
				case types::VALUE_STRING:
					this->to_cxx_object = new std::string(*(std::string*)rhs.to_cxx_object);
					break;
				default:
					this->to_cxx_object = nullptr;
					break;
				}
			}
			else
				this->to_cxx_object = nullptr;
		}

		jsonobj::jsonobj(jsonobj&& rhs)
		{
			this->t = rhs.t;
			this->to_cxx_object = rhs.to_cxx_object;
			rhs.t = types::VALUE_NULL;
			rhs.to_cxx_object = nullptr;
		}

		jsonobj::jsonobj(types type)
		{
			this->t = type;
			switch (this->t)
			{
			case types::VALUE_ARRAY:
				this->to_cxx_object = new json_array();
				break;
			case types::VALUE_BOOL:
				this->to_cxx_object = new bool();
				break;
			case types::VALUE_DOUBLE:
				this->to_cxx_object = new double();
				break;
			case types::VALUE_INT:
				this->to_cxx_object = new int();
				break;
			case types::VALUE_OBJ:
				this->to_cxx_object = new json_obj();
				break;
			case types::VALUE_STRING:
				this->to_cxx_object = new std::string();
				break;
			default:
				this->to_cxx_object = nullptr;
				break;
			}
		}

		jsonobj::jsonobj(std::string& s)
		{
			this->t = types::VALUE_STRING;
			this->to_cxx_object = new std::string(s);
		}

		jsonobj::jsonobj(int i)
		{
			this->t = types::VALUE_INT;
			this->to_cxx_object = new int(i);
		}

		jsonobj::jsonobj(double d)
		{
			this->t = types::VALUE_DOUBLE;
			this->to_cxx_object = new double(d);
		}

		void jsonobj::insert_v(jsonobj j, std::string key)
		{
			if (this->t == VALUE_ARRAY)
				((json_array*)this->to_cxx_object)->push_back(j);
			else if (this->t == VALUE_OBJ)
			{
				if (((json_obj*)this->to_cxx_object)->find(key) != ((json_obj*)this->to_cxx_object)->end())
					throw json_type_error("unexpected dupicate key found");
				(*(json_obj*)this->to_cxx_object)[key] = j;
			}
			else
				return; // error out here
			return;
		}

		void jsonobj::set_type(types _t)
		{
			if (this->to_cxx_object)
			{
				switch (this->t)
				{
				case types::VALUE_ARRAY:
					delete (json_array*)this->to_cxx_object;
					break;
				case types::VALUE_BOOL:
					delete (bool*)this->to_cxx_object;
					break;
				case types::VALUE_DOUBLE:
					delete (double*)this->to_cxx_object;
					break;
				case types::VALUE_INT:
					delete (int*)this->to_cxx_object;
					break;
				case types::VALUE_OBJ:
					delete (json_obj*)this->to_cxx_object;
					break;
				case types::VALUE_STRING:
					delete (std::string*)this->to_cxx_object;
					break;
				}
				this->to_cxx_object = nullptr;
			}

			switch (_t)
			{
			case types::VALUE_ARRAY:
				this->to_cxx_object = new json_array();
				break;
			case types::VALUE_BOOL:
				this->to_cxx_object = new bool();
				break;
			case types::VALUE_DOUBLE:
				this->to_cxx_object = new double();
				break;
			case types::VALUE_INT:
				this->to_cxx_object = new int();
				break;
			case types::VALUE_OBJ:
				this->to_cxx_object = new json_obj();
				break;
			case types::VALUE_STRING:
				this->to_cxx_object = new std::string();
				break;
			}
			this->t = _t;
		}

		std::map<std::string, jsonobj>& jsonobj::get_value_obj()
		{
			if (this->t != types::VALUE_OBJ)
				throw json_type_error("requesting a object in a non-object type");
			return *((json_obj*)this->to_cxx_object);
		}

		std::string& jsonobj::get_value_string()
		{
			if (this->t != types::VALUE_STRING)
				throw json_type_error("requesting a string in a non-string type");
			return *((std::string*)this->to_cxx_object);
		}

		std::vector<jsonobj>& jsonobj::get_value_array()
		{
			if (this->t != types::VALUE_ARRAY)
				throw json_type_error("requesting a array in a non-array type");
			return *((json_array*)this->to_cxx_object);
		}

		int& jsonobj::get_value_int()
		{
			if (this->t != types::VALUE_INT)
				throw json_type_error("requesting a int in a non-int type");
			return *((int*)this->to_cxx_object);
		}

		double& jsonobj::get_value_double()
		{
			if (this->t != types::VALUE_DOUBLE)
				throw json_type_error("requesting a double in a non-double type");
			return *((double*)this->to_cxx_object);
		}
		 
		bool& jsonobj::get_value_bool()
		{
			if (this->t != types::VALUE_BOOL)
				throw json_type_error("requesting a bool in a non-bool type");
			return *((bool*)this->to_cxx_object);
		}

		void jsonobj::clear()
		{
			switch (this->t)
			{
			case types::VALUE_ARRAY:
				((json_array*)this->to_cxx_object)->clear();
				break;
			case types::VALUE_BOOL:
				*((bool*)this->to_cxx_object) = 0;
				break;
			case types::VALUE_DOUBLE:
				*((double*)this->to_cxx_object) = 0;
				break;
			case types::VALUE_INT:
				*((int*)this->to_cxx_object) = 0;
				break;
			case types::VALUE_OBJ:
				((json_obj*)this->to_cxx_object)->clear();
				break;
			case types::VALUE_STRING:
				((std::string*)this->to_cxx_object)->clear();
				break;
			}
		}

		jsonobj jsonobj::operator= (const jsonobj& rhs)
		{
			this->~jsonobj();
			this->t = rhs.t;

			if (rhs.to_cxx_object)
			{
				switch (this->t)
				{
				case types::VALUE_ARRAY:
					this->to_cxx_object = new json_array(*(json_array*)rhs.to_cxx_object);
					break;
				case types::VALUE_BOOL:
					this->to_cxx_object = new bool(*(bool*)rhs.to_cxx_object);
					break;
				case types::VALUE_DOUBLE:
					this->to_cxx_object = new double(*(double*)rhs.to_cxx_object);
					break;
				case types::VALUE_INT:
					this->to_cxx_object = new int(*(int*)rhs.to_cxx_object);
					break;
				case types::VALUE_OBJ:
					this->to_cxx_object = new json_obj(*(json_obj*)rhs.to_cxx_object);
					break;
				case types::VALUE_STRING:
					this->to_cxx_object = new std::string(*(std::string*)rhs.to_cxx_object);
					break;
				default:
					this->to_cxx_object = nullptr;
					break;
				}
			}
			else
				this->to_cxx_object = nullptr;
			return *this;
		}
		
		jsonobj::~jsonobj()
		{
			if (this->to_cxx_object)
			{
				switch (this->t)
				{
				case types::VALUE_ARRAY:
					delete (json_array*)this->to_cxx_object;
					break;
				case types::VALUE_BOOL:
					delete (bool*)this->to_cxx_object; 
					break;
				case types::VALUE_DOUBLE:
					delete (double*)this->to_cxx_object;
					break;
				case types::VALUE_INT:
					delete (int*)this->to_cxx_object;
					break;
				case types::VALUE_OBJ:
					delete (json_obj*)this->to_cxx_object;
					break;
				case types::VALUE_STRING:
					delete (std::string*)this->to_cxx_object;
					break;
				}
				to_cxx_object = nullptr;
			}
		}

		int convert_numeric(std::string s, jsonobj& j)
		{
			bool t;
			double d;
			int i;
			if (s.find('.') != std::string::npos)
			{
				try
				{
					d = stod(s);
				}
				catch (...)
				{
					return -1;
				}
				j.set_type(types::VALUE_DOUBLE);
				j.get_value_double() = d;
			}
			else
			{
				try
				{
					i = stoi(s);
				}
				catch (...)
				{
					return -1;
				}
				j.set_type(types::VALUE_INT);
				j.get_value_int() = i;
			}
			return 0;
		}


	}
}