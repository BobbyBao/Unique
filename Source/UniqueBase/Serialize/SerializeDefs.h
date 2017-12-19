#pragma once
#include "../Core/EnumFlags.h"

enum class AttributeFlag : int
{
	Editable = 1,
	FileRead = 2,
	FileWrite = 4,
	Default = 7,
	Vector = 8,
	Map = 16,
};

ENABLE_BITMASK_OPERATORS(AttributeFlag)

#define uTransferField(NAME, FIELD, ...) transfer.TransferAttribute (NAME, FIELD, ##__VA_ARGS__);

#define uTransferAccessor(NAME, GET, SET, TYPE, ...)\
{\
	TYPE tmp;\
	if (transfer.IsWriting ())\
		tmp = GET(); \
	transfer.TransferAttribute(NAME, tmp, ##__VA_ARGS__); \
	if (transfer.IsReading ())\
		SET (tmp);\
}

#define uClass(...) \
	template<class TransferFunction> \
	void Transfer(TransferFunction& transfer)\
{\
transfer.TransferAttributes(##__VA_ARGS__);\
}


namespace Unique
{
	template<class T>
	class SerializeTraitsBase
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
	class SerializeTraits : public SerializeTraitsBase<T>
	{
	public:
		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.Transfer(data);
		}

	};

	template<class T>
	class SerializeTraitsPrimitive : public SerializeTraitsBase<T>
	{
	public:
		inline static bool IsBasicType() { return true; }

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferPrimitive(data);
		}

	};

	template<class T>
	struct NonConstContainerValueType
	{
		typedef typename T::value_type value_type;
	};

	template<class T, bool IsContinous = true>
	class SerializeTraitsArray : public SerializeTraitsBase<T>
	{
	public:
		inline static bool IsArray() { return true; }

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferArray(data);
		}

		static bool IsContinousMemoryArray() { return IsContinous; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { data.shrink_to_fit(); }
	};

	template<class T>
	class SerializeTraitsMap : public SerializeTraitsBase<T>
	{
	public:
		inline static bool IsMap() { return true; }

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferArray(data);
		}

		static bool IsContinousMemoryArray() { return false; }
	};

}
