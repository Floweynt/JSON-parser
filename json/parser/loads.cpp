// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "parser.h"
#include <stack>

#include "../json_obj/macros.h"

struct pack
{
	json::intern::jsonobj o;
	std::string key;
};

class parse_frame
{
	std::stack<pack> frames;
public:

	void merge_frames(json::intern::jsonobj& j)
	{
		pack p = frames.top();
		p.o.insert_v(j, p.key);
		frames.pop();
		std::swap(j, p.o);
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

namespace json
{
	namespace intern
	{
		int loads(const std::string& buf, json::intern::jsonobj& obj)
		{
			states s = OBJ;
			size_t layer = 0;

			parse_frame frames;
			json::intern::jsonobj root(json::types::VALUE_OBJ);

			std::string key;
			std::string val;
			bool is_esc = false;

			for (size_t i = 0; i < buf.size(); i++)
			{
				if (layer == 0 && i != 0)
					__RETURN__ (0)
				switch (s)
				{
				case OBJ:
					layer++;
					if (buf[i] != '{') 
						__RETURN__(ERR_NO_BASE_OBJ)
					else
						s = WAIT_FOR_KEY_OBJ;
					break;

					/**************** WAITS ****************/

				case WAIT_FOR_KEY:
					if (buf[i] == '\"')
						s = KEY;
					else if (isspace(buf[i]));
					else
						__RETURN__(ERR_UNEXPECTED_CHAR)
					break;
				case WAIT_FOR_KEY_OBJ:
					if (buf[i] == '\"')
						s = KEY;
					else if (isspace(buf[i]));
					else if (buf[i] == '}')
					{
						obj = root;
						__RETURN__(0)
					}
					else
						__RETURN__(ERR_UNEXPECTED_CHAR)
					break;
				case WAIT_FOR_COLON:
					if (buf[i] == ':')
						s = WAIT_FOR_VALUE;
					else if (isspace(buf[i]));
					else
						__RETURN__(ERR_UNEXPECTED_CHAR)
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
						root.set_type(types::VALUE_ARRAY);
						s = ARRAY_WAIT_FOR_VALUE;
					}
					else if (buf[i] == '{')
					{
						layer++;
						frames.push_frame(key, root);
						key.clear();
						root.clear();
						root.set_type(types::VALUE_OBJ);
						s = WAIT_FOR_KEY;
					}
					else if (isspace(buf[i]));
					else
						__RETURN__(ERR_UNEXPECTED_CHAR)
					break;
				case WAIT_FOR_COMMA:
					if (buf[i] == ',')
						s = WAIT_FOR_KEY;
					else if (buf[i] == '}')
					{
						layer--;
						if (layer != 0)
							frames.merge_frames(root);
						if (root.get_type() == types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == types::VALUE_OBJ)
							s = WAIT_FOR_COMMA;
					}
					else if (buf[i] == ']')
						__RETURN__(ERR_BRACKETS_MISMATCH)
					else if (isspace(buf[i]));
					else
						__RETURN__(ERR_UNEXPECTED_CHAR)
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
								__RETURN__(ERR_NOT_VALID_ESCAPE);
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
						val += buf[i];
					else if (isspace(buf[i]))
					{
						if(root.insert_numeric(val, key))
							__RETURN__(ERR_NOT_NUM)

						val.clear();
						key.clear();
						s = WAIT_FOR_COMMA;
					}
					else if (buf[i] == ',')
					{
						if (root.insert_numeric(val, key))
							__RETURN__(ERR_NOT_NUM)

						val.clear();
						key.clear();
						s = WAIT_FOR_KEY;
					}
					else if (buf[i] == '}')
					{
						layer--;
						if (layer != 0)
							frames.merge_frames(root);
						if (root.get_type() == types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == types::VALUE_OBJ)
							s = WAIT_FOR_COMMA;
					}
					else
						__RETURN__(ERR_UNEXPECTED_CHAR)
					break;
				case KEYWORD_VALUE:
					if (isspace(buf[i]))
					{
						intern::jsonobj value_bool;
						value_bool.set_type(types::VALUE_BOOL);
						if (val == "true")
							value_bool.get_value_bool() = true;
						else if (val == "false")
							value_bool.get_value_bool() = false;
						else if (val == "null")
							value_bool.set_type(types::VALUE_NULL);
						else
							__RETURN__(ERR_UNEXPECTED_CHAR)
						root.insert_v(value_bool, key);
						key.clear();
						s = WAIT_FOR_COMMA;
					}
					else if (buf[i] == ',')
					{
						intern::jsonobj value_bool;
						value_bool.set_type(types::VALUE_BOOL);
						if (val == "true")
							value_bool.get_value_bool() = true;
						else if (val == "false")
							value_bool.get_value_bool() = false;
						else if (val == "null")
							value_bool.set_type(types::VALUE_NULL);
						else
							__RETURN__(ERR_UNEXPECTED_CHAR)
						root.insert_v(value_bool, key);
						key.clear();
						val.clear();
						s = WAIT_FOR_KEY;
					}
					else if (buf[i] == '}')
					{
						layer--;
						if (layer != 0)
							frames.merge_frames(root);
						if (root.get_type() == types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == types::VALUE_OBJ)
							s = WAIT_FOR_COMMA;
					}
					else
						val += buf[i];
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
						root.set_type(types::VALUE_ARRAY);
						s = ARRAY_WAIT_FOR_VALUE;
					}
					else if (buf[i] == '{')
					{
						layer++;
						frames.push_frame(key, root);
						key.clear();
						root.clear();
						root.set_type(types::VALUE_OBJ);
						s = WAIT_FOR_KEY_OBJ;
					}
					else if (isspace(buf[i]));
					else
						__RETURN__(ERR_UNEXPECTED_CHAR)
					break;
				case ARRAY_WAIT_FOR_COMMA:
					if (buf[i] == ',')
						s = ARRAY_WAIT_FOR_VALUE;
					else if (buf[i] == ']')
					{
						layer--;
						if (layer != 0)
							frames.merge_frames(root);
						if (root.get_type() == types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == types::VALUE_OBJ)
							s = WAIT_FOR_COMMA;
					}
					else if (buf[i] == '}')
						__RETURN__(ERR_BRACKETS_MISMATCH)
					else if (isspace(buf[i]));
					else
						__RETURN__(ERR_UNEXPECTED_CHAR)
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
					break;
				case ARRAY_NUMERIC_VALUE:
					if (isdigit(buf[i]) || buf[i] == '-' || buf[i] == '+' || buf[i] == 'e' || buf[i] == 'E' || buf[i] == '.')
						val += buf[i];
					else if (isspace(buf[i]))
					{
						if (root.insert_numeric(val, key))
							__RETURN__(ERR_NOT_NUM)

						val.clear();
						key.clear();
						s = ARRAY_WAIT_FOR_COMMA;
					}
					else if (buf[i] == ',')
					{
						if (root.insert_numeric(val, key))
							__RETURN__(ERR_NOT_NUM)

						val.clear();
						key.clear();
						s = ARRAY_WAIT_FOR_VALUE;
					}
					else if (buf[i] == ']')
					{
						layer--;
						if (layer != 0)
							frames.merge_frames(root);
						if (root.get_type() == types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == types::VALUE_OBJ)
							s = WAIT_FOR_COMMA;
					}
					break;
				case ARRAY_KEYWORD_VALUE:
					if (isspace(buf[i]))
					{
						intern::jsonobj value_bool;
						value_bool.set_type(types::VALUE_BOOL);
						if (val == "true")
							value_bool.get_value_bool() = true;
						else if (val == "false")
							value_bool.get_value_bool() = false;
						else if (val == "null")
							value_bool.set_type(types::VALUE_NULL);
						else
							__RETURN__(ERR_UNEXPECTED_CHAR)
						root.insert_v(value_bool, key);
						key.clear();
						s = ARRAY_WAIT_FOR_COMMA;
					}
					else if (buf[i] == ',')
					{
						intern::jsonobj value_bool;
						value_bool.set_type(types::VALUE_BOOL);
						if (val == "true")
							value_bool.get_value_bool() = true;
						else if (val == "false")
							value_bool.get_value_bool() = false;
						else if (val == "null")
							value_bool.set_type(types::VALUE_NULL);
						else
							__RETURN__(ERR_UNEXPECTED_CHAR)
						root.insert_v(value_bool, key);
						key.clear();
						val.clear();
						s = ARRAY_WAIT_FOR_VALUE;
					}
					else if (buf[i] == ']')
					{
						layer--;
						if (layer != 0)
							frames.merge_frames(root);
						if (root.get_type() == types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == types::VALUE_OBJ)
							s = WAIT_FOR_COMMA;
					}
					break;
				}
				if (layer == 0)
					break;
			}

			if (layer != 0)
				__RETURN__(ERR_BRACKETS_MISMATCH)
			std::swap(root, obj);
			__RETURN__(0)
		}
	}
}