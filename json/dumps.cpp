// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "json.h"
#include <stack>

namespace json
{
	struct dumps_frame
	{
		json::JSONobj::iterator begin;
		json::JSONobj::iterator begin;
	};

	int JSONobj::dumps(std::string& buf, bool compact, size_t tab)
	{
		size_t layer = 1;
		json_types t;

		switch (this->get_type)
		{
		case VALUE_OBJ:
			t = json_types::VALUE_OBJ;
			buf = "{ ";
			break;
		case VALUE_ARRAY:
			t = json_types::VALUE_ARRAY;
			buf = "[ ";
			break;
		default:
			return -1;
		}

		JSONobj::iterator obj_begin = this->begin();
		JSONobj::iterator obj_end = this->begin();
		std::stack<dumps_frame> frames;

		while (true)
		{
			bool is_end = obj_begin == obj_end;

			if (is_end && layer == 1)
				break;
			else if (is_end)
			{
				dumps_frame it = frames.top();
				frames.pop();

				if (t == VALUE_ARRAY)
				{
					if (it.type == json_types::VALUE_ARRAY)
					{
						array_end = it.array_end;
						array_iter = it.array_iter;
						array_iter++;
					}
					else if (it.type == json_types::VALUE_OBJ)
					{
						obj_end = it.obj_end;
						obj_iter = it.obj_iter;
						obj_iter++;
					}
					buf += " ]";
					if (it.type == json_types::VALUE_ARRAY)
						buf += array_iter != array_end ? ", " : "";
					else if (it.type == json_types::VALUE_OBJ)
						buf += obj_iter != obj_end ? ", " : "";
				}
				else if (t == VALUE_OBJ)
				{
					if (it.type == json_types::VALUE_ARRAY)
					{
						array_end = it.array_end;
						array_iter = it.array_iter;
						array_iter++;
					}
					else if (it.type == json_types::VALUE_OBJ)
					{
						obj_end = it.obj_end;
						obj_iter = it.obj_iter;
						obj_iter++;
					}
					buf += " }";
					if (it.type == json_types::VALUE_ARRAY)
						buf += array_iter != array_end ? ", " : "";
					else if (it.type == json_types::VALUE_OBJ)
						buf += obj_iter != obj_end ? ", " : "";
				}
				t = it.type;
				layer--;
			}
			else
			{
				json_types s;
				if (t == VALUE_ARRAY)
					s = array_iter->get_type();
				else if (t == VALUE_OBJ)
					s = obj_iter->second.get_type();
				switch (s)
				{
				case VALUE_STRING:
					if (t == json_types::VALUE_OBJ)
					{
						buf += '\"' + obj_iter->first + "\": " + "\"" + obj_iter->second.get_value_string() + "\"";
						obj_iter++;
						if (obj_iter != obj_end)
							buf += ", ";
					}
					else if (t == json_types::VALUE_ARRAY)
					{
						buf += '\"' + array_iter->get_value_string() + "\"";
						array_iter++;
						if (array_iter != array_end)
							buf += ", ";
					}
					break;
				case VALUE_INT:
					if (t == json_types::VALUE_OBJ)
					{
						buf += '\"' + obj_iter->first + "\": " + std::to_string(obj_iter->second.get_value_int());
						obj_iter++;
						if (obj_iter != obj_end)
							buf += ", ";
					}
					else if (t == json_types::VALUE_ARRAY)
					{
						buf += std::to_string(array_iter->get_value_int());
						array_iter++;
						if (array_iter != array_end)
							buf += ", ";
					}
					break;
				case VALUE_DOUBLE:
					if (t == json_types::VALUE_OBJ)
					{
						buf += '\"' + obj_iter->first + "\": " + std::to_string(obj_iter->second.get_value_double());
						obj_iter++;
						if (obj_iter != obj_end)
							buf += ", ";
					}
					else if (t == json_types::VALUE_ARRAY)
					{
						buf += std::to_string(array_iter->get_value_double());
						array_iter++;
						if (array_iter != array_end)
							buf += ", ";
					}
					break;
				case VALUE_BOOL:
					if (t == json_types::VALUE_OBJ)
					{
						buf += '\"' + obj_iter->first + "\": " + (obj_iter->second.get_value_bool() ? "true" : "false");
						obj_iter++;
						if (obj_iter != obj_end)
							buf += ", ";
					}
					else if (t == json_types::VALUE_ARRAY)
					{
						buf += (array_iter->get_value_bool() ? "true" : "false");
						array_iter++;
						if (array_iter != array_end)
							buf += ", ";
					}
					break;
				case VALUE_NULL:
					// do stuff
					buf += '\"' + obj_iter->first + "\": null";
					obj_iter++;
					if (obj_iter != obj_end)
						buf += ", ";
					break;
				case VALUE_OBJ:
					if (t == json_types::VALUE_OBJ) // is the current object obj?
					{
						buf += "\"" + obj_iter->first + "\": { ";
						iter it;
						it.obj_end = obj_end;
						it.obj_iter = obj_iter;
						it.type = t;
						obj_end = obj_iter->second.get_value_obj().end();
						obj_iter = obj_iter->second.get_value_obj().begin();
						frames.push(it);
						t = json_types::VALUE_OBJ;
					}
					else if (t == json_types::VALUE_ARRAY) // is the current object array?
					{
						buf += "{ ";
						iter it;
						it.array_end = array_end;
						it.array_iter = array_iter;
						it.type = t;
						obj_end = array_iter->get_value_obj().end();
						obj_iter = array_iter->get_value_obj().begin();
						frames.push(it);
					}
					layer++;
					t = json_types::VALUE_OBJ;
					break;
				case VALUE_ARRAY:
					if (t == json_types::VALUE_OBJ) // is the current object obj?
					{
						buf += "\"" + obj_iter->first + "\": [ "; // add the the starting array string with key
						iter it;
						it.obj_end = obj_end;						// push current iterator end
						it.obj_iter = obj_iter;						// push current iterator
						it.type = t;								// set type
						array_end = obj_iter->second.get_value_array().end();
						array_iter = obj_iter->second.get_value_array().begin();
						frames.push(it);
					}
					else if (t == json_types::VALUE_ARRAY) // is the current object array?
					{
						buf += "[ "; // add the the starting array string with key
						iter it;
						it.array_end = array_end;						// push current iterator end
						it.array_iter = array_iter;						// push current iterator
						it.type = t;									// set type
						array_end = array_iter->get_value_array().end();
						array_end = array_iter->get_value_array().begin();
						frames.push(it);
					}
					layer++;
					t = json_types::VALUE_ARRAY;
					break;
				}
			}
		}
		buf += " }";
		return 0;
	}
}