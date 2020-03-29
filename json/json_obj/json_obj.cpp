#include "json_obj.h"

namespace CFP
{
	namespace json
	{
		std::ostream& operator<<(std::ostream& os, const JSONobj& out)
		{
			if (((JSONobj)out).object.get_type() == types::VALUE_STRING)
				os << ((JSONobj)out).object.get_value_string();
			return os;
		}
	}
}