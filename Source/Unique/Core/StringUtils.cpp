#include "Precompiled.h"
#include "StringUtils.h"


namespace Unique
{

	String FormatString(const char* formatString, ...)
	{
		String ret;
		va_list args;
		va_start(args, formatString);
		ret.AppendWithFormatArgs(formatString, args);
		va_end(args);
		return ret;
	}

}