#include "json_obj_internal.h"
#include <ostream>

namespace CFP
{
	namespace json
	{
		class JSONobj
		{
			intern::jsonobj object;


		public:
			inline JSONobj() { }
			JSONobj(intern::jsonobj o) { this->object = o; }
			inline operator std::string() { return object.get_value_string(); }

			JSONobj operator [](std::string index) { return this->object.get_value_obj()[index]; }
			JSONobj operator [](int index) { return this->object.get_value_array()[index]; }

			friend std::ostream& operator<<(std::ostream& os, const JSONobj& out);
			bool operator ==(JSONobj& rhs);
		};
	}
}