#pragma once
#include "../Container/Str.h"
#include "../Math/Color.h"
#include "../Math/Matrix3.h"
#include "../Math/Matrix3x4.h"
#include "../Math/Rect.h"

namespace Unique
{
	String FormatString(const char* formatString, ...);
	bool Match(const String& str, const String& pattern, bool caseSensitive);
	ByteArray ToBase64(ByteArray& v);
	ByteArray FromBase64(const String& base64);
	ByteArray FromBase64(const char* base64, size_t size);

	/// Parse a Color from a string.
	UNIQUE_API Color ToColor(const String& source);
	/// Parse a Color from a C string.
	UNIQUE_API Color ToColor(const char* source);
	/// Parse an IntRect from a string.
	UNIQUE_API IntRect ToIntRect(const String& source);
	/// Parse an IntRect from a C string.
	UNIQUE_API IntRect ToIntRect(const char* source);
	/// Parse an IntVector2 from a string.
	UNIQUE_API IntVector2 ToIntVector2(const String& source);
	/// Parse an IntVector2 from a C string.
	UNIQUE_API IntVector2 ToIntVector2(const char* source);
	/// Parse an IntVector3 from a string.
	UNIQUE_API IntVector3 ToIntVector3(const String& source);
	/// Parse an IntVector3 from a C string.
	UNIQUE_API IntVector3 ToIntVector3(const char* source);
	/// Parse a Quaternion from a string. If only 3 components specified, convert Euler angles (degrees) to quaternion.
	UNIQUE_API Quaternion ToQuaternion(const String& source);
	/// Parse a Quaternion from a C string. If only 3 components specified, convert Euler angles (degrees) to quaternion.
	UNIQUE_API Quaternion ToQuaternion(const char* source);
	/// Parse a Rect from a string.
	UNIQUE_API Rect ToRect(const String& source);
	/// Parse a Rect from a C string.
	UNIQUE_API Rect ToRect(const char* source);
	/// Parse a Vector2 from a string.
	UNIQUE_API Vector2 ToVector2(const String& source);
	/// Parse a Vector2 from a C string.
	UNIQUE_API Vector2 ToVector2(const char* source);
	/// Parse a Vector3 from a string.
	UNIQUE_API Vector3 ToVector3(const String& source);
	/// Parse a Vector3 from a C string.
	UNIQUE_API Vector3 ToVector3(const char* source);
	/// Parse a Vector4 from a string.
	UNIQUE_API Vector4 ToVector4(const String& source, bool allowMissingCoords = false);
	/// Parse a Vector4 from a C string.
	UNIQUE_API Vector4 ToVector4(const char* source, bool allowMissingCoords = false);
	/// Parse a Matrix3 from a string.
	UNIQUE_API Matrix3 ToMatrix3(const String& source);
	/// Parse a Matrix3 from a C string.
	UNIQUE_API Matrix3 ToMatrix3(const char* source);
	/// Parse a Matrix3x4 from a string.
	UNIQUE_API Matrix3x4 ToMatrix3x4(const String& source);
	/// Parse a Matrix3x4 from a C string.
	UNIQUE_API Matrix3x4 ToMatrix3x4(const char* source);
	/// Parse a Matrix4 from a string.
	UNIQUE_API Matrix4 ToMatrix4(const String& source);
	/// Parse a Matrix4 from a C string.
	UNIQUE_API Matrix4 ToMatrix4(const char* source);

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

	template<> inline String FromString<String>(const String& str) { return str; }
	template<> inline std::string FromString<std::string>(const String& str) { return str.CString(); }
	template<> inline bool FromString<bool>(const String& str) { return str.Compare("true", false) == 0; }
	template<> inline char FromString<char>(const String& str) { return (char)std::strtol(str.CString(), nullptr, 0); }
	template<> inline byte FromString<byte>(const String& str) { return (byte)std::strtoul(str.CString(), nullptr, 0); }
	template<> inline short FromString<short>(const String& str) { return (short)std::strtol(str.CString(), nullptr, 0); }
	template<> inline ushort FromString<ushort>(const String& str) { return (ushort)std::strtoul(str.CString(), nullptr, 0); }
	template<> inline int FromString<int>(const String& str) { return std::strtol(str.CString(), nullptr, 0); }
	template<> inline uint FromString<uint>(const String& str) { return std::strtoul(str.CString(), nullptr, 0); }
	template<> inline long FromString<long>(const String& str) { return std::strtol(str.CString(), nullptr, 0); }
	template<> inline unsigned long FromString<unsigned long>(const String& str) { return std::strtoul(str.CString(), nullptr, 0); }
	template<>inline float FromString<float>(const String& str){	return (float)std::strtof(str.CString(), nullptr);}
	template<>inline double FromString<double>(const String& str) { return std::strtod(str.CString(), nullptr); }
	
	template <> inline Color FromString<Color>(const String& source) { return ToColor(source); }
	template <> inline IntRect FromString<IntRect>(const String& source) { return ToIntRect(source); }
	template <> inline IntVector2 FromString<IntVector2>(const String& source) { return ToIntVector2(source); }
	template <> inline IntVector3 FromString<IntVector3>(const String& source) { return ToIntVector3(source); }
	template <> inline Quaternion FromString<Quaternion>(const String& source) { return ToQuaternion(source); }
	template <> inline Rect FromString<Rect>(const String& source) { return ToRect(source); }
	template <> inline Vector2 FromString<Vector2>(const String& source) { return ToVector2(source); }
	template <> inline Vector3 FromString<Vector3>(const String& source) { return ToVector3(source); }
	template <> inline Vector4 FromString<Vector4>(const String& source) { return ToVector4(source); }
	template <> inline Matrix3 FromString<Matrix3>(const String& source) { return ToMatrix3(source); }
	template <> inline Matrix3x4 FromString<Matrix3x4>(const String& source) { return ToMatrix3x4(source); }
	template <> inline Matrix4 FromString<Matrix4>(const String& source) { return ToMatrix4(source); }
}

