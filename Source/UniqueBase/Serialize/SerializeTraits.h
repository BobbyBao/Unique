#pragma once
#include "../Core/CoreDefs.h"
#include "../Container/Ptr.h"
#include "SerializeTraitsBasic.h"
#include "SerializeTraitsStd.h"
	

namespace Unique
{

	template<class T>
	class SerializeTraits<Unique::SPtr<T> > : public SerializeTraitsBase<Unique::SPtr<T> >
	{
	public:

		inline static bool AllowTransferOptimization() { return T::AllowTransferOptimization(); }
		inline static const char* GetTypeName() { return T::GetTypeStatic().c_str(); }
		inline static bool IsObject() { return true; }

		inline static bool CreateObject(Unique::SPtr<T>& obj, const char* type)
		{
			obj = Unique::StaticCast<T, Unique::Object>(Unique::Object::GetContext()->CreateObject(type));
			return true;
		}

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferObject(data);
		}

	};

	template<class T>
	class SerializeTraits<Vector<T> > : public SerializeTraitsArray<Vector<T> >
	{
	public:
		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferArray(data);
		}

		static bool IsContinousMemoryArray() { return true; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { resize_trimmed(data, rs); }
	};

	template<>
	class SerializeTraits<Vector<byte>> : public SerializeTraitsArray<Vector<byte>>
	{
	public:	
		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBin(data);
		}
	};

	template<>
	class SerializeTraits<ByteArray> : public SerializeTraitsArray<ByteArray>
	{
	public:
		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBin(data);
		}

	};

	template<class T>
	class SerializeTraitsEnum : public SerializeTraitsBase<T>
	{
	public:

		static int GetEnum(const char* enumNames[], int count, const Unique::String& val)
		{
			for (int i = 0; i < count; i++)
			{
				if (val.Compare(enumNames[i], false) == 0)
				{
					return i;
				}
			}

			return 0;
		}

		template<class TransferFunction, int count>
		inline static void TransferEnum(value_type& data, const char*(&enumNames)[count], TransferFunction& transfer)
		{
			if (transfer.IsReading())
			{
				Unique::String str;
				transfer.TransferPrimitive(str);
				data = (value_type)GetEnum(enumNames, count, str);
			}
			else
			{
				transfer.TransferPrimitive(Unique::String(enumNames[(int)data]));
			}
		}

	};
	
	template<class T>
	class SerializeTraitsFlags : public SerializeTraitsBase<T>
	{
	public:

		struct FlagPair
		{
			long enumValue;
			const char* enumName;
		};

		template<class TransferFunction>
		inline static void TransferFlags(value_type& data, Map<const char*, long> flags, TransferFunction& transfer)
		{
			Unique::String str;
			if (transfer.IsReading())
			{
				transfer.TransferPrimitive(str);
				Vector<String> strs = str.Split("|");
				for(auto& s : strs)
				{
					auto it = flags.find(s);
					if(it != flags.end())
					{
						data |= it.second;
					}
				}
			//	data = (value_type)GetEnum(enumNames, count, str);
			}
			else
			{
				int i = 0;
				for(auto it : flags)
				{
					if(data & it.second)
					{
						if(i != 0)
						{
							str.Append('|');
						}
						str.Append(it.first);
						i++;
					}
				}

				transfer.TransferPrimitive(str);
			}
		}

	};
}

#define uFlagsTraits(CLASS, ...)\
template<>\
class SerializeTraits<CLASS> : public SerializeTraitsFlags<CLASS>\
{\
public:\
	typedef CLASS value_type; \
	template<class TransferFunction>\
	inline static void Transfer(value_type& data, TransferFunction& transfer)\
	{\
		static Map<long, const char*> flags = \
		{\
			__VA_ARGS__\
		};\
		TransferFlags<TransferFunction>(data, flags, transfer); \
	}\
};

#define uEnumTraits(CLASS, ...)\
template<>\
class SerializeTraits<CLASS> : public SerializeTraitsEnum<CLASS>\
{\
public:\
	typedef CLASS value_type; \
	template<class TransferFunction>\
	inline static void Transfer(value_type& data, TransferFunction& transfer)\
	{\
		static const char* enumNames[] = \
		{\
				__VA_ARGS__\
		}; \
		TransferEnum<TransferFunction>(data, enumNames, transfer); \
	}\
};

#define uClassTraits(CLASS, ...)\
template<>\
class SerializeTraits<CLASS> : public SerializeTraitsBase<CLASS>\
{\
public:\
	typedef CLASS value_type; \
	template<class TransferFunction>\
	inline static void Transfer(value_type& self, TransferFunction& transfer)\
	{\
transfer.TransferAttributes(##__VA_ARGS__);\
	}\
};
