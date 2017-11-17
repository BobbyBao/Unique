#pragma once


#include "SerializeTraitsBasic.h"
#include "SerializeTraitsStd.h"
	

namespace Unique
{

	template<class T>
	class SerializeTraits<Unique::SPtr<T> > : public SerializeTraitsBase<Unique::SPtr<T> >
	{
	public:
		typedef Unique::SPtr<T> value_type;

		inline static const char* GetTypeString(void* ptr) { return value_type::GetTypeString(); }
		static bool AllowTransferOptimization() { return T::AllowTransferOptimization(); }
		inline static bool IsObject() { return true; }

		inline static bool CreateObject(Unique::SPtr<T>& obj, const char* type)
		{
			obj = Unique::StaticCast<T, Unique::Object>(Unique::Object::GetContext()->CreateObject(type));
			return true;
		}

		template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferObject(data);
		}

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

		template<class TransferFunction> inline
		static void TransferEnum(value_type& data, const char* enumNames[], int count, TransferFunction& transfer)
		{
			if (transfer.IsReading())
			{
				Unique::String str;
				transfer.TransferBasicData(str);
				data = (value_type)GetEnum(enumNames, count, str);
			}
			else
			{
				transfer.TransferBasicData(Unique::String(enumNames[data]));
			}
		}

	};

}