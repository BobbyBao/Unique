#pragma once
#include "../Unique.h"

namespace Unique
{
	class String;
	using ByteArray = Vector<char>;

	ByteArray&& toBase64(ByteArray& v);

	ByteArray&& fromBase64(const char* base64, size_t size);
}
