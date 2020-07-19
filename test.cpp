// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "json/json.h"

#include <iostream>
#include <chrono> 

using namespace json;

inline void ptestcase_err(int num, int ret)
{
	std::cout << "\x1b[31mfailed\x1b[0m: testcase " << num << " fail with return value of: " << ret << std::endl
		<< "testcase is located in the file testcase/testcase" << num << ".json" << std::endl;
	exit(1);
}

inline void passed(int testcase)
{
	 std::cout << "\x1b[32mpassed\x1b[0m: testcase " << testcase << std::endl;
}

//#define PRINT_EXIT

int main()
{
	auto start = std::chrono::steady_clock::now();
	try
	{
		JSON parser;
		JSONobj obj;
		int ret;

		ret = parser.deserialize_file("testcase/testcase1.json", obj);

		if (obj["testcase"] != "abc" 
			|| ret != 0
			)
			ptestcase_err(1, ret);
		passed(1);

		//parser.serialize_stream(std::cout, obj, 4);

		ret = parser.deserialize_file("testcase/testcase2.json", obj);
		if (obj["number"] != 55 ||
			obj["float"] != 123.4 ||
			ret != 0
			)
			ptestcase_err(2, ret);
		passed(2);

		ret = parser.deserialize_file("testcase/testcase3.json", obj);
		if (obj["names"][0] != "Joe" ||
			obj["names"][2] != "Sans" ||
			obj["names"][3] != "Chara" ||
			ret != 0
			)
			ptestcase_err(3, ret);
		passed(3);

		ret = parser.deserialize_file("testcase/testcase4.json", obj);
		if (obj["matrix"][0][0] != 1 ||
			obj["matrix"][0][1] != 2 ||
			obj["matrix"][1][1] != 4 ||
			ret != 0
			)
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
		if (!obj["A"] ||
			obj["B"] ||
			ret != 0
			)
			ptestcase_err(7, ret);
		passed(7);

		ret = parser.deserialize_file("testcase/testcase8.json", obj);
		if (ret != 0)
			ptestcase_err(8, ret);
		passed(8);

		ret = parser.deserialize_file("testcase/testcase9.json", obj);
		if (ret != 0)
			ptestcase_err(9, ret);
		passed(9);

		ret = parser.deserialize_file("testcase/testcase10.json", obj);
		if (obj["web-app"]["servlet"][0]["servlet-class"] != "org.cofax.cds.CDSServlet" ||
			obj["web-app"]["servlet"][0]["init-param"]["templateProcessorClass"] != "org.cofax.WysiwygTemplate" ||
			ret != 0
			)
			ptestcase_err(10, ret);
		passed(10);

		ret = parser.deserialize_file("testcase/testcase11.json", obj);
		if (obj["glossary"]["title"] != "example glossary" ||
			obj["glossary"]["GlossDiv"]["title"] != "S" ||
			ret != 0
			)
			ptestcase_err(11, ret);
		passed(11);
#ifdef _DEBUG
		std::cout << "COPY CTOR CALLS: " << json::intern::cctorc << ", COPY OPER CALLS: " << json::intern::eoperc;
#endif
		ret = parser.deserialize_file("testcase/testcase12.json", obj);
	}
	catch (const json::json_error& e)
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
	std::cout << "\n\x1b[36mtime elapsed\x1b[0m: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "ns" << std::endl;

	return 0;
}