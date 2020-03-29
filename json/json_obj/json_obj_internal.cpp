#include "json_obj_internal.h"
namespace CFP
{
	namespace json
	{
		namespace intern
		{
			jsonobj::jsonobj()
			{
				this->t = types::VALUE_NULL;
				this->value_double = 0;
				this->value_int = 0;
			}

			jsonobj::jsonobj(types type)
			{
				this->t = type;
				this->value_double = 0;
				this->value_int = 0;
			}

			jsonobj::jsonobj(std::string s)
			{
				this->t = types::VALUE_STRING;
				this->value_string = s;
				this->value_double = 0;
				this->value_int = 0;
			}

			void jsonobj::insert_v(jsonobj j, std::string key)
			{
				if (this->t == VALUE_ARRAY)
					this->value_array.push_back(j);
				else if (this->t == VALUE_OBJ)
				{
					if (this->value_obj.find(key) != this->value_obj.end())
						throw json_type_error("unexpected dupicate key found");
					this->value_obj[key] = j;
				}
				else
					return; // error out here
				return;
			}

			std::map<std::string, jsonobj>& jsonobj::get_value_obj()
			{
				if (this->t != types::VALUE_OBJ)
					throw json_type_error("requesting a object in a non-object type");
				return this->value_obj;
			}

			std::string& jsonobj::get_value_string()
			{
				if (this->t != types::VALUE_STRING)
					throw json_type_error("requesting a string in a non-string type");
				return this->value_string;
			}

			std::vector<jsonobj>& jsonobj::get_value_array()
			{
				if (this->t != types::VALUE_ARRAY)
					throw json_type_error("requesting a array in a non-array type");
				return this->value_array;
			}

			void jsonobj::clear()
			{
				this->value_array.clear();
				this->value_double = 0;
				this->value_int = 0;
				this->value_obj.clear();
				this->value_string.clear();
			}
		}
	}
}