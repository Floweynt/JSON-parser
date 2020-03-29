#include "json/json.h"
#include <iostream>
#include <iomanip>
using namespace CFP::json;
int main()
{
	CFPjson j;

	std::string example = "{ \"testcase\": \"abc\", \"hello\": \"world\", \"Joe\": {\"address\": \"4000 Foo Street\", \"lastname\": \"Smith\"}}";
	JSONobj obj;

	j.deserialize(example, obj);

	std::cout << "testcase: " << obj["testcase"] << std::endl
		<< "hello: " << obj["hello"] << std::endl
		<< "Joe's info:" << std::endl
		<< "\taddress: " << obj["Joe"]["address"] << std::endl
		<< "\tlastname: " << obj["Joe"]["lastname"] << std::endl;
}