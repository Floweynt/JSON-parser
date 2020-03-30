#ifndef  __CFP_JSON_PARSER_H__
#define __CFP_JSON_PARSER_H__
#include "json_obj/json_obj.h"
#include <stack>
#include <iostream>
namespace CFP
{
	namespace json
	{
		// error codes
		enum error_code
		{
			ERR_NO_BASE_OBJ = 1,
			ERR_UNEXPECTED_NON_SPACE,
			ERR_BRACKETS_MISMATCH
		};
		class CFPjson // parser object
		{
			enum states // parser states
			{
				OBJ,
				WAIT_FOR_KEY,
				KEY,
				WAIT_FOR_COLON,
				WAIT_FOR_VALUE,
				STRING_VALUE,
				WAIT_FOR_COMMA,
				OBJ_VALUE,
				ARRAY_VALUE,
				ARRAY_WAIT_FOR_COMMA,
				ARRAY_WAIT_FOR_VALUE,
			};

			struct pack
			{
				intern::jsonobj o;
				std::string key;
			};

			class parse_frame
			{
				std::stack<pack> frames;
			public:
				intern::jsonobj merge_frames(intern::jsonobj j)
				{
					pack p = frames.top();
					p.o.insert_v(j, p.key);
					frames.pop();
					frames.push(p);
					std::cout << "merging";
					return p.o;
				}

				void push_frame(std::string key, intern::jsonobj o)
				{
					pack p;
					p.o = o;
					p.key = key;
					frames.push(p);
				}
			};

		public:
			int deserialize(std::string buf, JSONobj& obj);
			int deserialize_file(std::string filename, JSONobj& obj);
		};


	}
}
#endif