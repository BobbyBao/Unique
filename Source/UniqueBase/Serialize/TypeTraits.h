#pragma once
#include "../Core/EnumFlags.h"

enum AttributeFlag : int
{
	AF_EDITABLE = 1,
	AF_FILEREAD = 2,
	AF_FILEWRITE = 4,
	AF_DEFAULT = 7,
	AF_VECTOR = 8,
	AF_MAP = 16,
};

uBitMask(AttributeFlag)

#define uVisitField(NAME, FIELD, ...) visitor.VisitAttribute (NAME, FIELD, ##__VA_ARGS__);

#define uVisitAccessor(NAME, GET, SET, TYPE, ...)\
{\
	TYPE tmp;\
	if (visitor.IsWriting ())\
		tmp = GET(); \
	visitor.VisitAttribute(NAME, tmp, ##__VA_ARGS__); \
	if (visitor.IsReading ())\
		SET (tmp);\
}

#define uClass(...) \
	template<class VisitFunction> \
	void Visit(VisitFunction& visitor)\
{\
visitor.VisitAttributes(##__VA_ARGS__);\
}


namespace Unique
{
	template<class T>
	class TypeTraitsBase
	{
	public:
		typedef T value_type;

		static int GetByteSize() { return sizeof(value_type); }
		inline static const char* GetTypeName() { return "";}
		inline static bool IsBasicType() { return false; }
		inline static bool IsObject() { return false; }
		inline static bool IsArray() { return false; }
		inline static bool IsMap() { return false; }
		inline static bool IsCollection() { return IsArray()||IsMap(); }
		inline static bool CreateObject(T& obj, const char* type) { return false; }

	};

	template<class T>
	class TypeTraits : public TypeTraitsBase<T>
	{
	public:
		template<class VisitFunction>
		inline static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.Visit(data);
		}

	};

	template<class T>
	class PrimitiveTraits : public TypeTraitsBase<T>
	{
	public:
		inline static bool IsBasicType() { return true; }

		template<class VisitFunction>
		inline static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitPrimitive(data);
		}

	};

	template<class T>
	struct NonConstContainerValueType
	{
		typedef typename T::value_type value_type;
	};

	template<class T, bool IsContinous = true>
	class SerializeTraitsArray : public TypeTraitsBase<T>
	{
	public:
		inline static bool IsArray() { return true; }

		template<class VisitFunction>
		inline static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitArray(data);
		}

		static bool IsContinousMemoryArray() { return IsContinous; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { data.shrink_to_fit(); }
	};

	template<class T>
	class SerializeTraitsMap : public TypeTraitsBase<T>
	{
	public:
		inline static bool IsMap() { return true; }

		template<class VisitFunction>
		inline static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitArray(data);
		}

		static bool IsContinousMemoryArray() { return false; }
	};

}
