#ifndef  __CFP_JSON_PARSER_H__
#define __CFP_JSON_PARSER_H__
#include "json_obj/json_obj.h"

namespace CFP
{
	namespace json
	{
		// error codes
		enum error_code
		{
			ERR_NO_BASE_OBJ = 1,
			ERR_UNEXPECTED_CHAR,
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
				ARRAY_WAIT_FOR_VALUE,
				ARRAY_WAIT_FOR_COMMA,
				NUMERIC_VALUE,
				ARRAY_STRING_VALUE,
				ARRAY_NUMERIC_VALUE
			};

		public:
			int deserialize(std::string buf, JSONobj& obj);
			int deserialize_file(std::string filename, JSONobj& obj);
		};


	}
}
#endif