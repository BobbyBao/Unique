#pragma once
#include "../Core/CoreDefs.h"
#include "../Container/Ptr.h"
#include "PrimitiveTypeTraits.h"
#include "StdTypeTraits.h"
	

namespace Unique
{
	template<class T>
	class TypeTraits<Unique::SPtr<T> > : public TypeTraitsBase<Unique::SPtr<T> >
	{
	public:
		inline static const char* GetTypeName() { return T::GetTypeStatic().c_str(); }
		inline static bool IsObject() { return true; }

		inline static bool CreateObject(Unique::SPtr<T>& obj, const char* type)
		{
			obj = Unique::StaticCast<T, Unique::Object>(Unique::Object::GetContext()->CreateObject(type));
			return true;
		}

		template<class VisitFunction>
		inline static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitObject(data);
		}

	};

// 	template<class T>
// 	class TypeTraits<Vector<T> > : public SerializeTraitsArray<Vector<T> >
// 	{
// 	public:
// 		template<class VisitFunction>
// 		inline static void Visit(value_type& data, VisitFunction& visitor)
// 		{
// 			visitor.VisitArray(data);
// 		}
// 	};
	
	template<>
	class TypeTraits<ByteArray> : public SerializeTraitsArray<ByteArray>
	{
	public:
		template<class VisitFunction>
		inline static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitBin(data);
		}

	};

	template<class T>
	class SerializeTraitsEnum : public TypeTraitsBase<T>
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

		template<class VisitFunction, int count>
		inline static void VisitEnum(value_type& data, const char*(&enumNames)[count], VisitFunction& visitor)
		{
			if (visitor.IsReading())
			{
				Unique::String str;
				visitor.VisitPrimitive(str);
				data = (value_type)GetEnum(enumNames, count, str);
			}
			else
			{
				visitor.VisitPrimitive(Unique::String(enumNames[(int)data]));
			}
		}

	};
	
	template<class T>
	class SerializeTraitsFlags : public TypeTraitsBase<T>
	{
	public:
		template<class VisitFunction>
		inline static void VisitFlags(value_type& data, const HashMap<String, value_type>& flags, VisitFunction& visitor)
		{
			Unique::String str;
			if (visitor.IsReading())
			{
				visitor.VisitPrimitive(str);
				auto i = flags.find(str.CString());
				if (i != flags.end())
				{
					data = i->second;
				}
				else
				{
					Vector<String> strs = str.Split('|');
					for (auto& s : strs)
					{
						auto it = flags.find(s);
						if (it != flags.end())
						{
							data |= it->second;
						}
					}
				}

			}
			else
			{
				int i = 0;
				for(auto& kvp : flags)
				{
					if (data == kvp.second)
					{
						str = kvp.first;
						break;
					}
					else
					{
						if (data & kvp.second)
						{
							if (i != 0)
							{
								str.Append('|');
							}
							str.Append(kvp.first);
							i++;
						}
					}
				
				}

				visitor.VisitPrimitive(str);
			}
		}

	};
}

#define uFlags(CLASS, ...)\
uBitMask(CLASS)\
template<>\
class TypeTraits<CLASS> : public SerializeTraitsFlags<CLASS>\
{\
public:\
	template<class VisitFunction>\
	inline static void Visit(value_type& data, VisitFunction& visitor)\
	{\
		static HashMap<String, value_type> flags = \
		{\
			__VA_ARGS__\
		};\
		VisitFlags<VisitFunction>(data, flags, visitor); \
	}\
};

#define uEnum(CLASS, ...)\
template<>\
class TypeTraits<CLASS> : public SerializeTraitsEnum<CLASS>\
{\
public:\
	template<class VisitFunction>\
	inline static void Visit(value_type& data, VisitFunction& visitor)\
	{\
		static const char* enumNames[] = \
		{\
				__VA_ARGS__\
		}; \
		VisitEnum<VisitFunction>(data, enumNames, visitor); \
	}\
};

#define uClassTraits(CLASS, ...)\
template<>\
class TypeTraits<CLASS> : public TypeTraitsBase<CLASS>\
{\
public:\
	template<class VisitFunction>\
	inline static void Visit(value_type& self, VisitFunction& visitor)\
	{\
visitor.VisitAttributes(##__VA_ARGS__);\
	}\
};
