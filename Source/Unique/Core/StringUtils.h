#pragma once
#include "../Container/Str.h"

namespace Unique
{
	template<typename T>
	inline String ToString(const T& v)
	{
		return String(v);
	}

	template<typename T>
	inline T FromString(const String& str)
	{
		return T();
	}
	
	template<> inline int FromString<int>(const String& str) { return std::strtol(str.CString(), nullptr, 0); }
	template<> inline uint FromString<uint>(const String& str) { return std::strtoul(str.CString(), nullptr, 0); }
	template<> inline long FromString<long>(const String& str) { return std::strtol(str.CString(), nullptr, 0); }
	template<> inline unsigned long FromString<unsigned long>(const String& str) { return std::strtoul(str.CString(), nullptr, 0); }
	template<>inline float FromString<float>(const String& str){	return (float)std::strtof(str.CString(), nullptr);}
	template<>inline double FromString<double>(const String& str) { return std::strtod(str.CString(), nullptr); }

}

