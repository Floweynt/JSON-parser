// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "json/json.h"
#include <iostream>
#include <chrono> 
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
	auto start = std::chrono::steady_clock::now();
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

		ret = parser.deserialize_file("testcase/testcase3.json", obj);
		if (obj["names"][0] != "Joe" || obj["names"][2] != "Sans" || obj["names"][3] != "Chara" || ret != 0)
			ptestcase_err(3, ret);
		passed(3);

		ret = parser.deserialize_file("testcase/testcase4.json", obj);
		if (obj["matrix"][0][0] != 1 || obj["matrix"][0][1] != 2 || obj["matrix"][1][1] != 4 || ret != 0)
			ptestcase_err(4, ret);
		passed(4);

		ret = parser.deserialize_file("testcase/testcase_fail1.json", obj);
		if (ret != ERR_BRACKETS_MISMATCH)
			ptestcase_err(5, ret);
		passed(5);

		ret = parser.deserialize_file("testcase/testcase_null.json", obj);
		if (ret != 0)
			ptestcase_err(6, ret);
		passed(6);

		ret = parser.deserialize_file("testcase/testcase7.json", obj);
		if (obj["A"] || obj)
	}
	catch (const CFP::CFP_error & e)
	{
		std::cout << "\x1b[31mfailed\x1b[0m: exception thrown with a what of:" << std::endl
			<< e.what() << std::endl;
		return -1;
	}
	catch (...)
	{
		std::cout << "\x1b[31mfailed\x1b[0m: exception thrown" << std::endl;
		return -1;
	}

	auto end = std::chrono::steady_clock::now();

	std::cout << "\x1b[36mtime elapsed\x1b[0m: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;
	return 0;
}