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

	//-----------------------------------------------------------------------
	bool Match(const String& str, const String& pattern, bool caseSensitive)
	{
		String tmpStr;
		String tmpPattern;
		if (!caseSensitive)
		{
			tmpStr = str.ToLower();
			tmpPattern = pattern.ToLower();
		}
		else
		{
			tmpStr = str;
			tmpPattern = pattern;
		}

		String::const_iterator strIt = tmpStr.Begin();
		String::const_iterator patIt = tmpPattern.Begin();
		String::const_iterator lastWildCardIt = tmpPattern.End();
		while (strIt != tmpStr.End() && patIt != tmpPattern.End())
		{
			if (*patIt == '*')
			{
				lastWildCardIt = patIt;
				// Skip over looking for next character
				++patIt;
				if (patIt == tmpPattern.End())
				{
					// Skip right to the end since * matches the entire rest of the string
					strIt = tmpStr.End();
				}
				else
				{
					// scan until we find next pattern character
					while (strIt != tmpStr.End() && *strIt != *patIt)
						++strIt;
				}
			}
			else
			{
				if (*patIt != *strIt)
				{
					if (lastWildCardIt != tmpPattern.End())
					{
						// The last wildcard can match this incorrect sequence
						// rewind pattern to wildcard and keep searching
						patIt = lastWildCardIt;
						lastWildCardIt = tmpPattern.End();
					}
					else
					{
						// no wildwards left
						return false;
					}
				}
				else
				{
					++patIt;
					++strIt;
				}
			}

		}
		// If we reached the end of both the pattern and the string, we succeeded
		if ((patIt == tmpPattern.End() || (*patIt == '*' && patIt + 1 == tmpPattern.End())) && strIt == tmpStr.End())
		{
			return true;
		}
		else
		{
			return false;
		}

	}

	ByteArray ToBase64(ByteArray& v)
	{
		const char alphabet[] = "ABCDEFGH" "IJKLMNOP" "QRSTUVWX" "YZabcdef"
			"ghijklmn" "opqrstuv" "wxyz0123" "456789+/";
		const char padchar = '=';
		int padlen = 0;

		ByteArray tmp;
		tmp.resize((v.size() * 4) / 3 + 3);

		int i = 0;
		char *out = tmp.data();
		while (i < v.size()) {
			int chunk = 0;
			chunk |= int(byte(v[i++])) << 16;
			if (i == v.size()) {
				padlen = 2;
			}
			else {
				chunk |= int(byte(v[i++])) << 8;
				if (i == v.size()) padlen = 1;
				else chunk |= int(byte(v[i++]));
			}

			int j = (chunk & 0x00fc0000) >> 18;
			int k = (chunk & 0x0003f000) >> 12;
			int l = (chunk & 0x00000fc0) >> 6;
			int m = (chunk & 0x0000003f);
			*out++ = alphabet[j];
			*out++ = alphabet[k];
			if (padlen > 1) *out++ = padchar;
			else *out++ = alphabet[l];
			if (padlen > 0) *out++ = padchar;
			else *out++ = alphabet[m];
		}

		tmp.resize(out - tmp.data());
		return tmp;
	}

	ByteArray FromBase64(const String& base64)
	{
		return FromBase64(base64.CString(), base64.Length());
	}

	ByteArray FromBase64(const char* base64, size_t size)
	{
		unsigned int buf = 0;
		int nbits = 0;
		ByteArray tmp;
		tmp.reserve((size * 3) / 4);

		int offset = 0;
		for (int i = 0; i < size; ++i) {
			int ch = base64[i];
			int d;

			if (ch >= 'A' && ch <= 'Z')
				d = ch - 'A';
			else if (ch >= 'a' && ch <= 'z')
				d = ch - 'a' + 26;
			else if (ch >= '0' && ch <= '9')
				d = ch - '0' + 52;
			else if (ch == '+')
				d = 62;
			else if (ch == '/')
				d = 63;
			else
				d = -1;

			if (d != -1) {
				buf = (buf << 6) | d;
				nbits += 6;
				if (nbits >= 8) {
					nbits -= 8;
					tmp.push_back(buf >> nbits);///*[offset++]*/ = buf >> nbits;
					buf &= (1 << nbits) - 1;
				}
			}
		}

		tmp.shrink_to_fit();
		return tmp;
	}

	unsigned CountElements(const char* buffer, char separator)
	{
		if (!buffer)
			return 0;

		const char* endPos = buffer + String::CStringLength(buffer);
		const char* pos = buffer;
		unsigned ret = 0;

		while (pos < endPos)
		{
			if (*pos != separator)
				break;
			++pos;
		}

		while (pos < endPos)
		{
			const char* start = pos;

			while (start < endPos)
			{
				if (*start == separator)
					break;

				++start;
			}

			if (start == endPos)
			{
				++ret;
				break;
			}

			const char* end = start;

			while (end < endPos)
			{
				if (*end != separator)
					break;

				++end;
			}

			++ret;
			pos = end;
		}

		return ret;
	}

	Color ToColor(const String& source)
	{
		return ToColor(source.CString());
	}

	Color ToColor(const char* source)
	{
		Color ret;

		unsigned elements = CountElements(source, ' ');
		if (elements < 3)
			return ret;

		char* ptr = (char*)source;
		ret.r_ = (float)strtod(ptr, &ptr);
		ret.g_ = (float)strtod(ptr, &ptr);
		ret.b_ = (float)strtod(ptr, &ptr);
		if (elements > 3)
			ret.a_ = (float)strtod(ptr, &ptr);

		return ret;
	}

	IntRect ToIntRect(const String& source)
	{
		return ToIntRect(source.CString());
	}

	IntRect ToIntRect(const char* source)
	{
		IntRect ret(IntRect::ZERO);

		unsigned elements = CountElements(source, ' ');
		if (elements < 4)
			return ret;

		char* ptr = (char*)source;
		ret.left_ = (int)strtol(ptr, &ptr, 10);
		ret.top_ = (int)strtol(ptr, &ptr, 10);
		ret.right_ = (int)strtol(ptr, &ptr, 10);
		ret.bottom_ = (int)strtol(ptr, &ptr, 10);

		return ret;
	}

	IntVector2 ToIntVector2(const String& source)
	{
		return ToIntVector2(source.CString());
	}

	IntVector2 ToIntVector2(const char* source)
	{
		IntVector2 ret(IntVector2::ZERO);

		unsigned elements = CountElements(source, ' ');
		if (elements < 2)
			return ret;

		char* ptr = (char*)source;
		ret.x_ = (int)strtol(ptr, &ptr, 10);
		ret.y_ = (int)strtol(ptr, &ptr, 10);

		return ret;
	}

	IntVector3 ToIntVector3(const String& source)
	{
		return ToIntVector3(source.CString());
	}

	IntVector3 ToIntVector3(const char* source)
	{
		IntVector3 ret(IntVector3::ZERO);

		unsigned elements = CountElements(source, ' ');
		if (elements < 3)
			return ret;

		char* ptr = (char*)source;
		ret.x_ = (int)strtol(ptr, &ptr, 10);
		ret.y_ = (int)strtol(ptr, &ptr, 10);
		ret.z_ = (int)strtol(ptr, &ptr, 10);

		return ret;
	}

	Rect ToRect(const String& source)
	{
		return ToRect(source.CString());
	}

	Rect ToRect(const char* source)
	{
		Rect ret(Rect::ZERO);

		unsigned elements = CountElements(source, ' ');
		if (elements < 4)
			return ret;

		char* ptr = (char*)source;
		ret.min_.x_ = (float)strtod(ptr, &ptr);
		ret.min_.y_ = (float)strtod(ptr, &ptr);
		ret.max_.x_ = (float)strtod(ptr, &ptr);
		ret.max_.y_ = (float)strtod(ptr, &ptr);

		return ret;
	}

	Quaternion ToQuaternion(const String& source)
	{
		return ToQuaternion(source.CString());
	}

	Quaternion ToQuaternion(const char* source)
	{
		unsigned elements = CountElements(source, ' ');
		char* ptr = (char*)source;

		if (elements < 3)
			return Quaternion::IDENTITY;
		else if (elements < 4)
		{
			// 3 coords specified: conversion from Euler angles
			float x, y, z;
			x = (float)strtod(ptr, &ptr);
			y = (float)strtod(ptr, &ptr);
			z = (float)strtod(ptr, &ptr);

			return Quaternion(x, y, z);
		}
		else
		{
			// 4 coords specified: full quaternion
			Quaternion ret;
			ret.w_ = (float)strtod(ptr, &ptr);
			ret.x_ = (float)strtod(ptr, &ptr);
			ret.y_ = (float)strtod(ptr, &ptr);
			ret.z_ = (float)strtod(ptr, &ptr);

			return ret;
		}
	}

	Vector2 ToVector2(const String& source)
	{
		return ToVector2(source.CString());
	}

	Vector2 ToVector2(const char* source)
	{
		Vector2 ret(Vector2::ZERO);

		unsigned elements = CountElements(source, ' ');
		if (elements < 2)
			return ret;

		char* ptr = (char*)source;
		ret.x_ = (float)strtod(ptr, &ptr);
		ret.y_ = (float)strtod(ptr, &ptr);

		return ret;
	}

	Vector3 ToVector3(const String& source)
	{
		return ToVector3(source.CString());
	}

	Vector3 ToVector3(const char* source)
	{
		Vector3 ret(Vector3::ZERO);

		unsigned elements = CountElements(source, ' ');
		if (elements < 3)
			return ret;

		char* ptr = (char*)source;
		ret.x_ = (float)strtod(ptr, &ptr);
		ret.y_ = (float)strtod(ptr, &ptr);
		ret.z_ = (float)strtod(ptr, &ptr);

		return ret;
	}

	Vector4 ToVector4(const String& source, bool allowMissingCoords)
	{
		return ToVector4(source.CString(), allowMissingCoords);
	}

	Vector4 ToVector4(const char* source, bool allowMissingCoords)
	{
		Vector4 ret(Vector4::ZERO);

		unsigned elements = CountElements(source, ' ');
		char* ptr = (char*)source;

		if (!allowMissingCoords)
		{
			if (elements < 4)
				return ret;

			ret.x_ = (float)strtod(ptr, &ptr);
			ret.y_ = (float)strtod(ptr, &ptr);
			ret.z_ = (float)strtod(ptr, &ptr);
			ret.w_ = (float)strtod(ptr, &ptr);

			return ret;
		}
		else
		{
			if (elements > 0)
				ret.x_ = (float)strtod(ptr, &ptr);
			if (elements > 1)
				ret.y_ = (float)strtod(ptr, &ptr);
			if (elements > 2)
				ret.z_ = (float)strtod(ptr, &ptr);
			if (elements > 3)
				ret.w_ = (float)strtod(ptr, &ptr);

			return ret;
		}
	}

	Matrix3 ToMatrix3(const String& source)
	{
		return ToMatrix3(source.CString());
	}

	Matrix3 ToMatrix3(const char* source)
	{
		Matrix3 ret(Matrix3::ZERO);

		unsigned elements = CountElements(source, ' ');
		if (elements < 9)
			return ret;

		char* ptr = (char*)source;
		ret.m00_ = (float)strtod(ptr, &ptr);
		ret.m01_ = (float)strtod(ptr, &ptr);
		ret.m02_ = (float)strtod(ptr, &ptr);
		ret.m10_ = (float)strtod(ptr, &ptr);
		ret.m11_ = (float)strtod(ptr, &ptr);
		ret.m12_ = (float)strtod(ptr, &ptr);
		ret.m20_ = (float)strtod(ptr, &ptr);
		ret.m21_ = (float)strtod(ptr, &ptr);
		ret.m22_ = (float)strtod(ptr, &ptr);

		return ret;
	}

	Matrix3x4 ToMatrix3x4(const String& source)
	{
		return ToMatrix3x4(source.CString());
	}

	Matrix3x4 ToMatrix3x4(const char* source)
	{
		Matrix3x4 ret(Matrix3x4::ZERO);

		unsigned elements = CountElements(source, ' ');
		if (elements < 12)
			return ret;

		char* ptr = (char*)source;
		ret.m00_ = (float)strtod(ptr, &ptr);
		ret.m01_ = (float)strtod(ptr, &ptr);
		ret.m02_ = (float)strtod(ptr, &ptr);
		ret.m03_ = (float)strtod(ptr, &ptr);
		ret.m10_ = (float)strtod(ptr, &ptr);
		ret.m11_ = (float)strtod(ptr, &ptr);
		ret.m12_ = (float)strtod(ptr, &ptr);
		ret.m13_ = (float)strtod(ptr, &ptr);
		ret.m20_ = (float)strtod(ptr, &ptr);
		ret.m21_ = (float)strtod(ptr, &ptr);
		ret.m22_ = (float)strtod(ptr, &ptr);
		ret.m23_ = (float)strtod(ptr, &ptr);

		return ret;
	}

	Matrix4 ToMatrix4(const String& source)
	{
		return ToMatrix4(source.CString());
	}

	Matrix4 ToMatrix4(const char* source)
	{
		Matrix4 ret(Matrix4::ZERO);

		unsigned elements = CountElements(source, ' ');
		if (elements < 16)
			return ret;

		char* ptr = (char*)source;
		ret.m00_ = (float)strtod(ptr, &ptr);
		ret.m01_ = (float)strtod(ptr, &ptr);
		ret.m02_ = (float)strtod(ptr, &ptr);
		ret.m03_ = (float)strtod(ptr, &ptr);
		ret.m10_ = (float)strtod(ptr, &ptr);
		ret.m11_ = (float)strtod(ptr, &ptr);
		ret.m12_ = (float)strtod(ptr, &ptr);
		ret.m13_ = (float)strtod(ptr, &ptr);
		ret.m20_ = (float)strtod(ptr, &ptr);
		ret.m21_ = (float)strtod(ptr, &ptr);
		ret.m22_ = (float)strtod(ptr, &ptr);
		ret.m23_ = (float)strtod(ptr, &ptr);
		ret.m30_ = (float)strtod(ptr, &ptr);
		ret.m31_ = (float)strtod(ptr, &ptr);
		ret.m32_ = (float)strtod(ptr, &ptr);
		ret.m33_ = (float)strtod(ptr, &ptr);

		return ret;
	}
}