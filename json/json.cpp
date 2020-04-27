// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "json.h"
#include <cstdio>
#include <stack>

struct pack
{
	json::intern::jsonobj o;
	std::string key;
};

class parse_frame
{
	std::stack<pack> frames;
public:

	json::intern::jsonobj merge_frames(json::intern::jsonobj j)
	{
		pack p = frames.top();
		p.o.insert_v(j, p.key);
		frames.pop();
		return p.o;
	}

	void push_frame(std::string key, json::intern::jsonobj o)
	{
		pack p;
		p.o = o;
		p.key = key;
		frames.push(p);
	}

	json::intern::jsonobj top()
	{
		return this->frames.top().o;
	}
};

enum states // parser states
{
	OBJ,

	WAIT_FOR_KEY,
	WAIT_FOR_KEY_OBJ,
	WAIT_FOR_COLON,
	WAIT_FOR_VALUE,
	WAIT_FOR_COMMA,

	KEY,

	STRING_VALUE,
	NUMERIC_VALUE,
	KEYWORD_VALUE,

	ARRAY_WAIT_FOR_VALUE,
	ARRAY_WAIT_FOR_COMMA,

	ARRAY_STRING_VALUE,
	ARRAY_NUMERIC_VALUE,
	ARRAY_KEYWORD_VALUE
};

struct iter
{
	std::vector<json::intern::jsonobj>::iterator array_iter;
	std::vector<json::intern::jsonobj>::iterator array_end;
	std::map<std::string, json::intern::jsonobj>::iterator obj_iter;
	std::map<std::string, json::intern::jsonobj>::iterator obj_end;
	json::types type;
};

namespace json
{
	int JSONparser::deserialize_file(const std::string& filename, JSONobj& obj)
	{
		FILE* fp = fopen(filename.c_str(), "r");
		if (fp == NULL)
			return -1;
		fseek(fp, 0, SEEK_END);
		size_t len = ftell(fp);
		rewind(fp);

		std::string buf;
		buf.resize(len);

		fread((void*)buf.c_str(), 1, len, fp);

		return this->deserialize(buf, obj);
		fclose(fp);
	}
	int JSONparser::deserialize(const std::string& buf, JSONobj& obj)
	{
		states s = OBJ;
		size_t layer = 0;

		parse_frame frames;
		intern::jsonobj root(types::VALUE_OBJ);

		JSONobj empty;
		obj = empty;

		std::string key;
		std::string val;
		std::string other;
		std::string num;
		bool is_esc = false;

		for (size_t i = 0; i < buf.size(); i++)
		{
			if (layer == 0 && i != 0)
				return 0;
			switch (s)
			{
			case OBJ:
				layer++;
				if (buf[i] != '{')
					return ERR_NO_BASE_OBJ;
				else
					s = WAIT_FOR_KEY_OBJ;
				break;

				/**************** WAITS ****************/

			case WAIT_FOR_KEY:
				if (buf[i] == '\"')
					s = KEY;
				else if (isspace(buf[i]));
				else
					return ERR_UNEXPECTED_CHAR;
				break;
			case WAIT_FOR_KEY_OBJ:
				if (buf[i] == '\"')
					s = KEY;
				else if (isspace(buf[i]));
				else if (buf[i] == '}')
				{
					obj = root;
					return 0;
				}
				else
					return ERR_UNEXPECTED_CHAR;
				break;
			case WAIT_FOR_COLON:
				if (buf[i] == ':')
					s = WAIT_FOR_VALUE;
				else if (isspace(buf[i]));
				else
					return ERR_UNEXPECTED_CHAR;
				break;
			case WAIT_FOR_VALUE:
				if (buf[i] == '\"')
					s = STRING_VALUE;
				else if (isdigit(buf[i]) || buf[i] == '-' || buf[i] == '+')
				{
					s = NUMERIC_VALUE;
					i--;
				}
				else if (buf[i] == 't' || buf[i] == 'f' || buf[i] == 'n')
				{
					s = KEYWORD_VALUE;
					i--;
				}
				else if (buf[i] == '[')
				{
					layer++;
					frames.push_frame(key, root);
					key.clear();
					root.clear();
					root.get_type() = types::VALUE_ARRAY;
					s = ARRAY_WAIT_FOR_VALUE;
				}
				else if (buf[i] == '{')
				{
					layer++;
					frames.push_frame(key, root);
					key.clear();
					root.clear();
					root.get_type() = types::VALUE_OBJ;
					s = WAIT_FOR_KEY;
				}
				else if (isspace(buf[i]));
				else
					return ERR_UNEXPECTED_CHAR;
				break;
			case WAIT_FOR_COMMA:
				if (buf[i] == ',')
					s = WAIT_FOR_KEY;
				else if (buf[i] == '}')
				{
					layer--;
					if (layer != 0)
						root = frames.merge_frames(root);
				}
				else if (buf[i] == ']')
					return ERR_BRACKETS_MISMATCH;
				else if (isspace(buf[i]));
				else
					return ERR_UNEXPECTED_CHAR;
				break;

				/**************** KEY ****************/

			case KEY:
				if (buf[i] == '\"' && !is_esc)
					s = WAIT_FOR_COLON;
				else
				{
					if (is_esc)
					{
						switch (buf[i])
						{
						case 'b':
							key += '\b';
							break;
						case 'f':
							key += '\f';
				 		break;
						case 'n':
							key += '\n';
						case 't':
							key += '\t';
							break;
						case '"':
							key += '"';
							break;
						case '\\':
							key += '\\';
						default:
							return -1;
						}
						is_esc = false;
					}
					else if (buf[i] == '\\')
						is_esc = true;
					else
						key += buf[i];
				}
				break;

				/**************** VALUES ****************/

			case STRING_VALUE:
				if (buf[i] == '\"')
				{
					s = WAIT_FOR_COMMA;
					root.insert_v(val, key);
					key.clear();
					val.clear();
				}
				else
					val += buf[i];
				break;
			case NUMERIC_VALUE:
				if (isdigit(buf[i]) || buf[i] == '-' || buf[i] == '+' || buf[i] == 'e' || buf[i] == 'E' || buf[i] == '.')
					num += buf[i];
				else if (isspace(buf[i]))
				{
					intern::jsonobj value_num;
					if (intern::convert_numeric(num, value_num) != 0)
						return -1;
					root.insert_v(value_num, key);
					num.clear();
					key.clear();
					s = WAIT_FOR_COMMA;
				}
				else if (buf[i] == ',')
				{
					intern::jsonobj value_num;
					if (intern::convert_numeric(num, value_num) != 0)
						return -1;
					root.insert_v(value_num, key);
					num.clear();
					key.clear();
					s = WAIT_FOR_KEY;
				}
				else
					return ERR_UNEXPECTED_CHAR;
				break;
			case KEYWORD_VALUE:
				if (isspace(buf[i]))
				{
					intern::jsonobj value_bool;
					value_bool.get_type() = types::VALUE_BOOL;
					if (other == "true")
						value_bool.get_value_bool() = true;
					else if (other == "false")
						value_bool.get_value_bool() = false;
					else if (other == "null")
						value_bool.get_type() = types::VALUE_NULL;
					else
						return ERR_UNEXPECTED_CHAR;
					root.insert_v(value_bool, key);
					key.clear();
					s = WAIT_FOR_COMMA;
				}
				else if (buf[i] == ',')
				{
					intern::jsonobj value_bool;
					value_bool.get_type() = types::VALUE_BOOL;
					if (other == "true")
						value_bool.get_value_bool() = true;
					else if (other == "false")
						value_bool.get_value_bool() = false;
					else if (other == "null")
						value_bool.get_type() = types::VALUE_NULL;
					else
						return ERR_UNEXPECTED_CHAR;
					root.insert_v(value_bool, key);
					key.clear();
					other.clear();
					s = WAIT_FOR_KEY;
				}
				else
					other += buf[i];
				break;

				/**************** ARRAY WAITS ****************/

			case ARRAY_WAIT_FOR_VALUE:
				if (buf[i] == '\"')
					s = ARRAY_STRING_VALUE;
				else if (isdigit(buf[i]) || buf[i] == '-' || buf[i] == '+')
				{
					s = ARRAY_NUMERIC_VALUE;
					i--;
				}
				else if (buf[i] == '[')
				{
					layer++;
					frames.push_frame(key, root);
					key.clear();
					root.clear();
					root.get_type() = types::VALUE_ARRAY;
					s = ARRAY_WAIT_FOR_VALUE;
				}
				else if (isspace(buf[i]));
				else
					return ERR_UNEXPECTED_CHAR;
				break;
			case ARRAY_WAIT_FOR_COMMA:
				if (buf[i] == ',')
					s = ARRAY_WAIT_FOR_VALUE;
				else if (buf[i] == ']')
				{
					layer--;
					if (layer != 0)
						root = frames.merge_frames(root);
					if (root.get_type() == types::VALUE_ARRAY)
						s = ARRAY_WAIT_FOR_COMMA;
					else if (root.get_type() == types::VALUE_OBJ)
						s = WAIT_FOR_COMMA;
				}
				else if (buf[i] == '}')
					return ERR_BRACKETS_MISMATCH;
				else if (isspace(buf[i]));
				else
					return ERR_UNEXPECTED_CHAR;
				break;

				/**************** ARRAY VALUES ****************/

			case ARRAY_STRING_VALUE:
				if (buf[i] == '\"')
				{
					s = ARRAY_WAIT_FOR_COMMA;
					root.insert_v(val, key);
					val.clear();
				}
				else
					val += buf[i];
			case ARRAY_NUMERIC_VALUE:
				if (isdigit(buf[i]) || buf[i] == '-' || buf[i] == '+' || buf[i] == 'e' || buf[i] == 'E' || buf[i] == '.')
					num += buf[i];
				else if (isspace(buf[i]))
				{
					intern::jsonobj value_num;
					if (intern::convert_numeric(num, value_num) != 0)
						return -1;
					root.insert_v(value_num, key);
					num.clear();
					key.clear();
					s = ARRAY_WAIT_FOR_COMMA;
				}
				else if (buf[i] == ',')
				{
					intern::jsonobj value_num;
					if (intern::convert_numeric(num, value_num) != 0)
						return -1;
					root.insert_v(value_num, key);
					num.clear();
					key.clear();
					s = ARRAY_WAIT_FOR_VALUE;
				}
				break;
			case ARRAY_KEYWORD_VALUE:
				if (isspace(buf[i]))
				{
					intern::jsonobj value_bool;
					value_bool.get_type() = types::VALUE_BOOL;
					if (other == "true")
						value_bool.get_value_bool() = true;
					else if (other == "false")
						value_bool.get_value_bool() = false;
					else if (other == "null")
						value_bool.get_type() = types::VALUE_NULL;
					else
						return ERR_UNEXPECTED_CHAR;
					root.insert_v(value_bool, key);
					key.clear();
					s = ARRAY_WAIT_FOR_COMMA;
				}
				else if (buf[i] == ',')
				{
					intern::jsonobj value_bool;
					value_bool.get_type() = types::VALUE_BOOL;
					if (other == "true")
						value_bool.get_value_bool() = true;
					else if (other == "false")
						value_bool.get_value_bool() = false;
					else if (other == "null")
						value_bool.get_type() = types::VALUE_NULL;
					else
						return ERR_UNEXPECTED_CHAR;
					root.insert_v(value_bool, key);
					key.clear();
					other.clear();
					s = ARRAY_WAIT_FOR_VALUE;
				}
				break;
			}
			if (layer == 0)
				break;
		}


		if (layer != 0)
			return ERR_BRACKETS_MISMATCH;
		obj = root;
		return 0;
	}

	int JSONparser::serialize(std::string& buf, const JSONobj& obj, bool compact, size_t tab)
	{
		size_t layer = 1;
		intern::jsonobj root = obj.get_internal();

		types t = types::VALUE_OBJ;
		auto obj_iter = root.get_value_obj().begin();
		auto obj_end = root.get_value_obj().end()--;
		std::vector<json::intern::jsonobj>::iterator array_iter;
		std::vector<json::intern::jsonobj>::iterator array_end;

		std::stack<iter> frames;

		buf = "{ ";
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
				else if (it.type == VALUE_OBJ)
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
					buf += " }";
					if (it.type == types::VALUE_ARRAY)
						buf += array_iter != array_end ? ", " : "";
					else if (it.type == types::VALUE_OBJ)
						buf += obj_iter != obj_end ? ", " : "";
				}
				t = it.type;
				layer--;
			}
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
					buf += '\"' + obj_iter->first + "\": " + "\"" + obj_iter->second.get_value_string() + "\"";
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
				else if (t == types::VALUE_ARRAY) // is the current object obj?
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
				else if (t == types::VALUE_ARRAY) // is the current object obj?
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
		buf += " }";
		return 0;
	}
}