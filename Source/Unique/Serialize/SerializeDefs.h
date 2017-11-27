#pragma once

enum class AttributeFlag
{
	Editable = 1,
	FileRead = 2,
	FileWrite = 4,
	Default = 7,
};

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

#define uClass(x, ...) \
	inline static const char* GetTypeString ()	{ return #x; }	\
	inline static bool AllowTransferOptimization ()	{ return false; } \
	template<class TransferFunction> \
	void Transfer(TransferFunction& transfer)\
{\
transfer.TransferAttributes(##__VA_ARGS__);\
}

#define DECLARE_SERIALIZE_OPTIMIZE_TRANSFER(x) \
	inline static const char* GetTypeString ()	{ return #x; }	\
	inline static bool AllowTransferOptimization ()	{ return true; } \
	template<class TransferFunction> \
	void Transfer (TransferFunction& transfer)

#define DEFINE_GET_TYPESTRING_BASICTYPE(x)						\
inline static const char* GetTypeString (void*)	{ return #x; } \
inline static bool AllowTransferOptimization ()	{ return true; }\
inline static bool IsBasicType() { return true; }

#define DEFINE_GET_TYPESTRING_CONTAINER(x)						\
inline static const char* GetTypeString (void*)	{ return #x; } \
inline static bool IsObject ()	{ return false; } \
inline static bool IsBasicType() { return SerializeTraits<T>::IsBasicType(); }

#define DEFINE_GET_TYPESTRING_MAP_CONTAINER(x)						\
inline static const char* GetTypeString (void*)	{ return #x; } \
inline static bool IsObject ()	{ return false; } \
inline static bool IsBasicType() { return false; }

namespace Unique
{
	template<class T>
	class SerializeTraitsBase
	{
	public:
		typedef T value_type;

		static int GetByteSize() { return sizeof(value_type); }

		inline static bool AllowTransferOptimization() { return false; }
		inline static bool IsBasicType() { return false; }
		inline static bool IsObject() { return false; }
		inline static bool CreateObject(T& obj, const char* type) { return false; }

	};

	template<class T>
	class SerializeTraits : public SerializeTraitsBase<T>
	{
	public:
		typedef T value_type;

		inline static const char* GetTypeString(void* ptr) { return value_type::GetTypeString(); }
		inline static bool AllowTransferOptimization() { return T::AllowTransferOptimization(); }

		template<class TransferFunction> inline
		static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.Transfer(data);
		}

	};


	template<class T>
	struct NonConstContainerValueType
	{
		typedef typename T::value_type value_type;
	};

}
