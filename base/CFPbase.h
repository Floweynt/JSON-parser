#ifndef __CFP_BASE_H__
#define __CFP_BASE_H__
#include <string>
#include <cstdio>
#include <stdexcept>
#include <map>
#include <vector>

namespace CFP
{
	using namespace std;

	class CFP_error : logic_error
	{
	protected:
		CFP_error(const char* p) : logic_error(p) {};
	};

	class base_error : CFP_error
	{
	public:
		base_error(const char* p) : CFP_error(p) {};
	};
}
#endif