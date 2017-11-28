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
		typedef Unique::SPtr<T> value_type;

		inline static bool AllowTransferOptimization() { return T::AllowTransferOptimization(); }
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
	class SerializeTraits<Vector<T> > : public SerializeTraitsBase<Vector<T> >
	{
	public:
		typedef Vector<T>	value_type;
		DEFINE_GET_TYPESTRING_CONTAINER(vector)

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferArray(data);
		}

		static bool IsContinousMemoryArray() { return true; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { resize_trimmed(data, rs); }
	};

	template<>
	class SerializeTraits<Vector<byte>> : public SerializeTraitsBase<Vector<byte>>
	{
	public:
		typedef Vector<byte>	value_type; 

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferArray(data);
		}

		static bool IsContinousMemoryArray() { return true; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { resize_trimmed(data, rs); }
	};

	template<class T>
	class SerializeTraitsEnum : public SerializeTraitsBase<T>
	{
	public:
		typedef T value_type;

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

		template<class TransferFunction, int count> inline
			static void TransferEnum(value_type& data, const char*(&enumNames)[count], TransferFunction& transfer)
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

}


#define uEnumTraits(CLASS, ...)\
	template<>\
	class SerializeTraits<CLASS> : public SerializeTraitsEnum<CLASS>\
	{\
	public:\
		typedef ShaderType value_type;\
		template<class TransferFunction>\
		inline static void Transfer(value_type& data, TransferFunction& transfer)\
		{\
			static const char* enumNames[] =\
			{\
				"VertexShader", "HullShader", "DomainShader", "GeometryShader",\
				"FragmentShader", "ComputeShader",\
			};\
			TransferEnum<TransferFunction>(data, enumNames, transfer);\
		}\
	};