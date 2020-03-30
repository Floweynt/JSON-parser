#include "json/json.h"
#include <iostream>
#include <iomanip>
using namespace CFP::json;

void ptestcase_err(int num, int ret, std::string e)
{
	std::cout << "testcase " << num << " fail with return value of: " << ret << std::endl
		<< "testcase is located in the file testcase/testcase" << num << ".json" << std::endl
		<< "expected result should be:" << std::endl
		<< e << std::endl;
	exit(1);
}

int main()
{
	CFPjson parser;
	JSONobj obj;
	int ret;

	ret = parser.deserialize_file("testcase/testcase1.json", obj);

	if ("abc" != (std::string)obj["testcase"] || ret != 0)
		ptestcase_err(1, ret, "expected obj[\"testcase\"] to be \"abc\"");
	return 0;
}