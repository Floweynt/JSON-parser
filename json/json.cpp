// SPDX-License-Identifier: MIT
// Copyright 2020 Ruiqi Li

#include "json.h"
#include "json_obj/macros.h"

#include <cstdio>
#include <stack>

namespace json
{
	int JSON::deserialize(const std::string& buf, JSONobj& obj)
	{
		intern::jsonobj myobj;
		int ret = intern::loads(buf, myobj);
		obj = myobj;
		return ret;
	}

	int JSON::deserialize_file(const std::string& filename, JSONobj& obj)
	{
		FILE* fp = fopen(filename.c_str(), "r");	// open file
		if (fp == NULL)								// error handling
			return -1;

		// get file size
		fseek(fp, 0, SEEK_END);
		size_t len = ftell(fp);
		rewind(fp);

		// read file
		std::string buf;
		buf.resize(len);

		fread((void*)buf.c_str(), 1, len, fp);
		fclose(fp);

		return this->deserialize(buf, obj);			// parse string
	}

	int JSON::deserialize_stream(std::istream& stream, JSONobj& obj)
	{
		std::string buf(std::istreambuf_iterator<char>(stream), {});	// read stream
		return this->deserialize(buf, obj);								// parse stream
	}

	int JSON::serialize(std::string& buf, const JSONobj& obj, bool compact, size_t tab) 
	{
		return intern::dumps(buf, obj.get_internal(), compact, tab);
	}

	int JSON::serialize_file(const std::string& filename, const JSONobj& obj, bool compact, size_t tab)
	{
		FILE* fp = fopen(filename.c_str(), "w");				// open file
		if (fp == NULL)											// error handling
			return -1;
		
		std::string tmp;
		int ret =  this->serialize(tmp, obj, compact, tab);		// parse string

		fwrite((void*)tmp.c_str(), 1, tmp.size(), fp);
		fclose(fp);
		return ret;
	}

	int JSON::serialize_stream(std::ostream& stream, const JSONobj& obj, bool compact, size_t tab)
	{
		std::string tmp;
		int ret = this->serialize(tmp, obj, compact, tab);		// parse string
		stream << tmp;

		return ret;
	}
}