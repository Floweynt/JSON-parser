#include "json/json.h"
#include <iostream>
#include <iomanip>
using namespace CFP::json;

void ptestcase_err(int num, int ret)
{
	std::cout << "\x1b[31mfailed\x1b[0m: testcase " << num << " fail with return value of: " << ret << std::endl
		<< "testcase is located in the file testcase/testcase" << num << ".json" << std::endl;
	exit(1);
}

void passed(int testcase)
{
	std::cout << "\x1b[32mpassed\x1b[0m: testcase " << testcase << std::endl;
}

int main()
{
	try
	{
		CFPjson parser;
		JSONobj obj;
		int ret;

		ret = parser.deserialize_file("testcase/testcase1.json", obj);

		if (obj["testcase"] != "abc" || ret != 0)
			ptestcase_err(1, ret);
		passed(1);

		ret = parser.deserialize_file("testcase/testcase2.json", obj);
		if (obj["number"] != 55 || obj["float"] != 123.4 || ret != 0)
			ptestcase_err(2, ret);
		passed(2);
		return 0;
	}
	catch (const CFP::CFP_error & e)
	{
		std::cout << "\x1b[31mfailed\x1b[0m: exception thrown with a what of:" << std::endl
			<< e.what();
	}
	catch (...)
	{
		std::cout << "\x1b[31mfailed\x1b[0m: exception thrown" << std::endl;
	}
}