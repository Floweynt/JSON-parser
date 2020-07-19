// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "parser.h"

#include <stack>

struct iter
{
	std::vector<json::intern::jsonobj>::iterator array_iter;
	std::vector<json::intern::jsonobj>::iterator array_end;
	std::map<std::string, json::intern::jsonobj>::iterator obj_iter;
	std::map<std::string, json::intern::jsonobj>::iterator obj_end;
	json::types type = json::types::VALUE_NULL;
};

namespace json
{
	namespace intern
	{
		static void gspace(std::string& in, const std::string& val, const size_t per, const size_t i)
		{
			in += std::string(per * i, ' ') += val + (per ? "\n" : "");
		}

		int dumps(std::string& buf, const jsonobj& obj, size_t tab)
		{
			size_t layer = 1;
			intern::jsonobj root = obj;
			types t;
			if (root.get_type() == VALUE_OBJ)
			{
				t = types::VALUE_OBJ;
				if (tab)
					buf = "{\n";
				else
					buf = "{ ";
			}
			else if (root.get_type() == VALUE_ARRAY)
			{
				t = types::VALUE_ARRAY;
				if (tab)
					buf = "[\n";
				else
					buf = "[ ";
			}
			else
				return -1;

			auto obj_iter = root.get_value_obj().begin();
			auto obj_end = root.get_value_obj().end();
			std::vector<json::intern::jsonobj>::iterator array_iter;
			std::vector<json::intern::jsonobj>::iterator array_end;

			std::stack<iter> frames;

			while (true)
			{
				bool is_end;
				if (t == types::VALUE_ARRAY)
					is_end = array_iter == array_end;
				else if (t == types::VALUE_OBJ)
					is_end = obj_iter == obj_end;

				if (is_end && layer == 1)
					break;
				else if (is_end)
				{
					iter it = frames.top();
					frames.pop();

					if (t == VALUE_ARRAY)
					{
						if (it.type == types::VALUE_ARRAY)
						{
							array_end = it.array_end;
							array_iter = it.array_iter;
							array_iter++;
						}
						else if (it.type == types::VALUE_OBJ)
						{
							obj_end = it.obj_end;
							obj_iter = it.obj_iter;
							obj_iter++;
						}
						buf += " ]";
						if (it.type == types::VALUE_ARRAY)
							buf += array_iter != array_end ? ", " : "";
						else if (it.type == types::VALUE_OBJ)
							buf += obj_iter != obj_end ? ", " : "";
					}
					else if (t == VALUE_OBJ)
					{
						if (it.type == types::VALUE_ARRAY)
						{
							array_end = it.array_end;
							array_iter = it.array_iter;
							array_iter++;
						}
						else if (it.type == types::VALUE_OBJ)
						{
							obj_end = it.obj_end;
							obj_iter = it.obj_iter;
							obj_iter++;
						}
						if (tab)
							buf += "}";
						else
							buf += " }";

						if (it.type == types::VALUE_ARRAY)
							buf += array_iter != array_end ? ", " : "";
						else if (it.type == types::VALUE_OBJ)
							buf += obj_iter != obj_end ? ", " : "";
					}
					t = it.type;
					layer--;
				}
				else
				{
					types s;
					if (t == VALUE_ARRAY)
						s = array_iter->get_type();
					else if (t == VALUE_OBJ)
						s = obj_iter->second.get_type();
					switch (s)
					{
					case VALUE_STRING:
						if (t == types::VALUE_OBJ)
						{
							gspace(buf, (std::string)"\"" + obj_iter->first + "\": " + "\"" + obj_iter->second.get_value_string() + "\"", tab, layer);
							obj_iter++;
							if (obj_iter != obj_end)
								buf += ", ";
						}
						else if (t == types::VALUE_ARRAY)
						{
							buf += '\"' + array_iter->get_value_string() + "\"";
							array_iter++;
							if (array_iter != array_end)
								buf += ", ";
						}
						break;
					case VALUE_INT:
						if (t == types::VALUE_OBJ)
						{
							buf += '\"' + obj_iter->first + "\": " + std::to_string(obj_iter->second.get_value_int());
							obj_iter++;
							if (obj_iter != obj_end)
								buf += ", ";
						}
						else if (t == types::VALUE_ARRAY)
						{
							buf += std::to_string(array_iter->get_value_int());
							array_iter++;
							if (array_iter != array_end)
								buf += ", ";
						}
						break;
					case VALUE_DOUBLE:
						if (t == types::VALUE_OBJ)
						{
							buf += '\"' + obj_iter->first + "\": " + std::to_string(obj_iter->second.get_value_double());
							obj_iter++;
							if (obj_iter != obj_end)
								buf += ", ";
						}
						else if (t == types::VALUE_ARRAY)
						{
							buf += std::to_string(array_iter->get_value_double());
							array_iter++;
							if (array_iter != array_end)
								buf += ", ";
						}
						break;
					case VALUE_BOOL:
						if (t == types::VALUE_OBJ)
						{
							buf += '\"' + obj_iter->first + "\": " + (obj_iter->second.get_value_bool() ? "true" : "false");
							obj_iter++;
							if (obj_iter != obj_end)
								buf += ", ";
						}
						else if (t == types::VALUE_ARRAY)
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
						if (t == types::VALUE_OBJ) // is the current object obj?
						{
							buf += "\"" + obj_iter->first + "\": { ";
							iter it;
							it.obj_end = obj_end;
							it.obj_iter = obj_iter;
							it.type = t;
							obj_end = obj_iter->second.get_value_obj().end();
							obj_iter = obj_iter->second.get_value_obj().begin();
							frames.push(it);
							t = types::VALUE_OBJ;
						}
						else if (t == types::VALUE_ARRAY) // is the current object array?
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
						t = types::VALUE_OBJ;
						break;
					case VALUE_ARRAY:
						if (t == types::VALUE_OBJ) // is the current object obj?
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
						else if (t == types::VALUE_ARRAY) // is the current object array?
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
						t = types::VALUE_ARRAY;
						break;
					}
				}
			}
			if (t == types::VALUE_OBJ)
			{
				if (tab)
					buf += "}";
				else
					buf += " }";
			}
			else if (t == types::VALUE_ARRAY)
			{
				if (tab)
					buf += "]";
				else
					buf += " ]";
			}
			return 0;
		}
	}
}