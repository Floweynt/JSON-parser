#ifndef __CFP_BASE_H__
#define __CFP_BASE_H__
#include <stdexcept>

namespace CFP
{
	class CFP_error : public std::logic_error
	{
	public:
		CFP_error(const char* p) : std::logic_error(p) {};
	};
}
#endif