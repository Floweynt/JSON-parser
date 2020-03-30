#include "json.h"
#include <cstdio>

#ifdef _WIN32
#pragma warning(disable: 4996) 
#endif


namespace CFP
{
	namespace json
	{
		int CFPjson::deserialize_file(std::string filename, JSONobj& obj)
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
		}
		int CFPjson::deserialize(std::string buf, JSONobj& obj)
		{
			states s = OBJ;
			size_t layer = 0;

			parse_frame frames;
			intern::jsonobj root(types::VALUE_OBJ);
			
			JSONobj empty;
			obj = empty;

			std::string key;
			std::string val;

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
						s = WAIT_FOR_KEY;
					break;
				case WAIT_FOR_KEY:
					if (buf[i] == '\"')
						s = KEY;
					else if (isspace(buf[i]));
					else
						return ERR_UNEXPECTED_NON_SPACE;
					break;
				case KEY:
					if (buf[i] == '\"')
						s = WAIT_FOR_COLON;
					else
						key += buf[i];
					break;
				case WAIT_FOR_COLON:
					if (buf[i] == ':')
						s = WAIT_FOR_VALUE;
					else if (isspace(buf[i]));
					else
						return ERR_UNEXPECTED_NON_SPACE;
					break;
				case WAIT_FOR_VALUE:
					if (buf[i] == '\"')
						s = STRING_VALUE;
					else if (buf[i] == '[')
						s = ARRAY_VALUE;
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
						return ERR_UNEXPECTED_NON_SPACE;
					break;
				case STRING_VALUE:
					if (buf[i] == '\"')
					{
						s = WAIT_FOR_COMMA;
						root.get_value_obj()[key] = val;
						key.clear();
						val.clear();
					}
					else
						val += buf[i];
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
					else if (isspace(buf[i]));
					else
						return ERR_UNEXPECTED_NON_SPACE;
					break;
				case ARRAY_WAIT_FOR_COMMA:
					if (buf[i] == ',')
						s = ARRAY_WAIT_FOR_VALUE;
					else if (isspace(buf[i]));
					else
						return ERR_UNEXPECTED_NON_SPACE;
				}
				if (layer == 0)
					break;
			}


			if (layer != 0)
				return ERR_BRACKETS_MISMATCH;
			obj = root;
			return 0;
		}
	}
}