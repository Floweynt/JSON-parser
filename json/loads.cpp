#include "json.h"
#include <stack>
#include <cmath>
#include <string>
using namespace json;

namespace json
{
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

	int insert_numeric_implicit(const std::string& str, const std::string& key, JSONobj& obj)
	{
		if (obj.is_array() || obj.is_object())
			return -1;

		double d;
		try
		{
			d = std::stod(str);
		}
		catch (...)
		{
			return 1;
		}

		if (str.find('.') != std::string::npos)
		{
			if (obj.is_object())
				obj.push_back(std::make_pair(key, (double)d));
			else if (obj.is_array())
				obj.push_back((double)d);
		}
		if (trunc(d) == d)
		{
			if (d > INT_MIN && d < INT_MAX)
			{
				if (obj.is_object())
					obj.push_back(std::make_pair(key, (JSONobj)d));
				else if (obj.is_array())
					obj.push_back((JSONobj)d);
			}
			else
			{
				if (obj.is_object())
					obj.push_back(std::make_pair(key, (JSONobj)d));
				else if (obj.is_array())
					obj.push_back((JSONobj)d);
			}
		}
		return 0;
	}

	class loads_frame
	{
		std::stack<JSONobj> oframes;
		std::stack<std::string> sframes;
	public:

		void merge_frames(json::JSONobj& j)
		{
			// we first move the top to a temp buffer
			pack p(std::move(frames.top()));
			// remove top of frames
			frames.pop();

			std::swap(p.o[p.key], j);
			std::swap(j, p.o);
		}

		
		void push_frame(const std::string& key, json::JSONobj& o)
		{
			frames.emplace(o, key);
			// destorying a JSONobj sets it to null type, but we use a interface
			o.nullify();
		}
		
	};

	int JSONobj::loads(const std::string& buf)
	{
		states s = OBJ;
		size_t layer = 0;

		loads_frame frames;
		JSONobj root;

		std::string key;
		std::string val;

		bool is_esc = false;

		for (size_t i = 0; i < buf.size(); i++)
		{
			if (layer == 0 && i != 0)
				__RETURN__(0)
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
						std::swap(*this, root);
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
//						frames.push_frame(key, root);
						key.clear();
						root.set_type(json_types::VALUE_ARRAY);
						s = ARRAY_WAIT_FOR_VALUE;
					}
					else if (buf[i] == '{')
					{
						layer++;
//						frames.push_frame(key, root);
						key.clear();
						root.set_type(json_types::VALUE_OBJ);
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
						if (root.get_type() == json_types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == json_types::VALUE_OBJ)
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
						root.push_back(std::make_pair(val, key));
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
						if (insert_numeric_implicit(val, key, root))
							__RETURN__(ERR_NOT_NUM)

						val.clear();
						key.clear();
						s = WAIT_FOR_COMMA;
					}
					else if (buf[i] == ',')
					{
						if (insert_numeric_implicit(val, key, root))
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
						if (root.get_type() == json_types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == json_types::VALUE_OBJ)
							s = WAIT_FOR_COMMA;
					}
					else
						__RETURN__(ERR_UNEXPECTED_CHAR)
						break;
				case KEYWORD_VALUE:
					if (isspace(buf[i]))
					{
						if (val == "true")
							root.push_back(std::make_pair(key, (json_bool)true));
						else if (val == "false")
							root.push_back(std::make_pair(key, (json_bool)true));
						else if (val == "null");
						// we uh, actually dont do anything
						else
							__RETURN__(ERR_UNEXPECTED_CHAR);
						key.clear();
						s = WAIT_FOR_COMMA;
					}
					else if (buf[i] == ',')
					{
						if (val == "true")
							root.push_back(std::make_pair(key, (json_bool)true));
						else if (val == "false")
							root.push_back(std::make_pair(key, (json_bool)true));
						else if (val == "null");
						// we uh, actually dont do anything
						else
							__RETURN__(ERR_UNEXPECTED_CHAR);
						key.clear();
						s = WAIT_FOR_KEY;
					}
					else if (buf[i] == '}')
					{
						layer--;
						if (layer != 0)
							frames.merge_frames(root);
						if (root.get_type() == json_types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == json_types::VALUE_OBJ)
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
//						frames.push_frame(key, root);
						root.set_type(json_types::VALUE_ARRAY);
						s = ARRAY_WAIT_FOR_VALUE;
					}
					else if (buf[i] == '{')
					{
						layer++;
//						frames.push_frame(key, root);
						root.set_type(json_types::VALUE_OBJ);
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
						if (root.get_type() == json_types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == json_types::VALUE_OBJ)
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
						root.push_back(key);
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
						if (insert_numeric_implicit(val, key, root))
							__RETURN__(ERR_NOT_NUM)

						val.clear();
						s = ARRAY_WAIT_FOR_COMMA;
					}
					else if (buf[i] == ',')
					{
						if (insert_numeric_implicit(val, key, root))
							__RETURN__(ERR_NOT_NUM)

						val.clear();
						s = ARRAY_WAIT_FOR_VALUE;
					}
					else if (buf[i] == ']')
					{
						layer--;
						if (layer != 0)
							frames.merge_frames(root);
						if (root.get_type() == json_types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == json_types::VALUE_OBJ)
							s = WAIT_FOR_COMMA;
					}
					break;
				case ARRAY_KEYWORD_VALUE:
					if (isspace(buf[i]))
					{
						if (val == "true")
							root.push_back(std::make_pair(key, (json_bool)true));
						else if (val == "false")
							root.push_back(std::make_pair(key, (json_bool)true));
						else if (val == "null");
						// we uh, actually dont do anything
						else
							__RETURN__(ERR_UNEXPECTED_CHAR);
						val.clear();

						s = ARRAY_WAIT_FOR_COMMA;
					}
					else if (buf[i] == ',')
					{
						if (val == "true")
							root.push_back(std::make_pair(key, (json_bool)true));
						else if (val == "false")
							root.push_back(std::make_pair(key, (json_bool)true));
						else if (val == "null");
						// we uh, actually dont do anything
						else
							__RETURN__(ERR_UNEXPECTED_CHAR);
						val.clear();

						s = ARRAY_WAIT_FOR_VALUE;
					}
					else if (buf[i] == ']')
					{
						layer--;
						if (layer != 0)
							frames.merge_frames(root);
						if (root.get_type() == json_types::VALUE_ARRAY)
							s = ARRAY_WAIT_FOR_COMMA;
						else if (root.get_type() == json_types::VALUE_OBJ)
							s = WAIT_FOR_COMMA;
					}
					break;
				}
			if (layer == 0)
				break;
		}

		if (layer != 0)
			__RETURN__(ERR_BRACKETS_MISMATCH)
			std::swap(*this, root);
		__RETURN__(0)
	}
}