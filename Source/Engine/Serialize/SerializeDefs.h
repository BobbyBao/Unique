#pragma once

enum TransferMetaFlags
{
	TF_DEFAULT = 0,
	TF_NOEDIT = 1,
	TF_READONLY = 2
};

#define uField(NAME, FIELD) transfer.TransferAttribute (NAME, FIELD, TF_DEFAULT);
#define TRANSFER_FLAG(NAME, FIELD, FLAG) transfer.TransferAttribute (NAME, FIELD, FLAG);

#define TRANSFER_PROPERTY(NAME, PROP, TYPE) TRANSFER_PROPERTY_FLAG(NAME, PROP, TYPE, TF_DEFAULT);
#define TRANSFER_PROPERTY_FLAG(NAME, PROP, TYPE, FLAG)\
{\
	TYPE tmp;\
	if (transfer.IsWriting ())\
		tmp = Get##PROP(); \
	transfer.Transfer(NAME, tmp, FLAG); \
	if (transfer.IsReading ())\
		Set##PROP(tmp);\
}

#define TRANSFER_ACCESSOR(NAME, GET, SET, TYPE) TRANSFER_ACCESSOR_FLAG(NAME, GET, SET, TYPE, TF_DEFAULT)

#define TRANSFER_ACCESSOR_FLAG(NAME, GET, SET, TYPE, FLAG)\
{\
	TYPE tmp;\
	if (transfer.IsWriting ())\
		tmp = GET(); \
	transfer.Transfer(NAME, tmp, FLAG); \
	if (transfer.IsReading ())\
		SET (tmp);\
}

#define TRANSFER_ENUM(NAME, FIELD) { assert(sizeof(FIELD) == sizeof(int)); transfer.Transfer (FIELD, NAME, TF_DEFAULT); }
#define TRANSFER_ENUM_FLAG(NAME, FIELD, FLAG) { assert(sizeof(FIELD) == sizeof(int)); transfer.Transfer (FIELD, NAME, FLAG); }
#define TRANSFER_ENUM_(FIELD) { assert(sizeof(FIELD##_) == sizeof(int)); transfer.Transfer (FIELD##_, #FIELD, TF_DEFAULT); }

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
