#ifndef SERIALIZETRAITS_H
#define SERIALIZETRAITS_H

#include "SerializeDefs.h"
#include "../Math/MathDefs.h"
#include "../Container/Str.h"

namespace Unique
{

	template<>
	class SerializeTraits<float> : public SerializeTraitsBase<float>
	{
	public:
		typedef float value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(float)

		template<class TransferFunction> inline
		static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};


	template<>
	class SerializeTraits<double> : public SerializeTraitsBase<double>
	{
	public:
		typedef double value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(double)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

	template<>
	class SerializeTraits<int> : public SerializeTraitsBase<int>
	{
	public:
		typedef int value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(int)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};


	template<>
	class SerializeTraits<unsigned int> : public SerializeTraitsBase<unsigned int>
	{
	public:
		typedef unsigned int value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(unsigned int)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};


	template<>
	class SerializeTraits<short> : public SerializeTraitsBase<short>
	{
	public:
		typedef short value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(short)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};


	template<>
	class SerializeTraits<unsigned short> : public SerializeTraitsBase<unsigned short>
	{
	public:
		typedef unsigned short value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(unsigned short)

		template<class TransferFunction> inline
		static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

	template<>
	class SerializeTraits<unsigned char> : public SerializeTraitsBase<unsigned char>
	{
	public:
		typedef unsigned char value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(unsigned char)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

	template<>
	class SerializeTraits<char> : public SerializeTraitsBase<char>
	{
	public:
		typedef char value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(char)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

	template<>
	class SerializeTraits<char*> : public SerializeTraitsBase<char*>
	{
	public:
		typedef char* value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(char*)

		template<class TransferFunction> inline
		static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

	template<>
	class SerializeTraits<bool> : public SerializeTraitsBase<bool>
	{
	public:
		typedef bool value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(bool)

			static int GetByteSize() { return 1; }

		template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
#if (defined __ppc__) && !UNITY_WII && !UNITY_CAFE
			AssertIf(sizeof(bool) != 4);
			unsigned char& temp = *(reinterpret_cast<unsigned char*>(&data) + 3);

			transfer.TransferBasicData(temp);

			// When running in debug mode in OS X (-O0 in gcc), 
			// bool values which are not exactly 0x01 are treated as false.
			// We don't want this. Cast UInt8 to bool to fix this.
			if (transfer.IsReading())
				data = temp;
#if DEBUGMODE
			AssertIf((transfer.IsReading() || transfer.IsWriting()) && (reinterpret_cast<int&> (data) != 0 && reinterpret_cast<int&> (data) != 1));
#endif
#else
			assert(sizeof(bool) == 1);
			//unsigned char& temp = reinterpret_cast<unsigned char&>(data);
			transfer.TransferBasicData(data);

			// When running in debug mode in OS X (-O0 in gcc), 
			// bool values which are not exactly 0x01 are treated as false.
			// We don't want this. Cast UInt8 to bool to fix this.
#if DEBUGMODE
			if (transfer.IsReading())
				data = temp;
			// You constructor or Reset function is not setting the bool value to a defined value!
			AssertIf((transfer.IsReading() || transfer.IsWriting()) && (temp != 0 && temp != 1));
#endif
#endif
		}
	};
	
	template<>
	class SerializeTraits<Unique::String> : public SerializeTraitsBase<Unique::String>
	{
	public:
		typedef Unique::String	value_type;
		inline static const char* GetTypeString(value_type*) { return "String"; }
		inline static bool IsBasicType() { return true; }

		template<class TransferFunction> inline
		static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}

		static bool IsContinousMemoryArray() { return true; }

		static void ResizeSTLStyleArray(value_type& data, int rs)
		{
			data.Resize(rs);
		}

	};


	template<>
	class SerializeTraits<Unique::Vector2> : public SerializeTraitsBase<Unique::Vector2>
	{
	public:

		typedef Unique::Vector2 value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(Vector2)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

	template<>
	class SerializeTraits<Unique::Vector3> : public SerializeTraitsBase<Unique::Vector3>
	{
	public:

		typedef Unique::Vector3 value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(Vector3)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

	template<>
	class SerializeTraits<Unique::Vector4> : public SerializeTraitsBase<Unique::Vector4>
	{
	public:

		typedef Unique::Vector4 value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(Vector4)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

	template<>
	class SerializeTraits<Unique::Color> : public SerializeTraitsBase<Unique::Color>
	{
	public:

		typedef Unique::Color value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(Color)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

	template<>
	class SerializeTraits<Unique::Quaternion> : public SerializeTraitsBase<Unique::Quaternion>
	{
	public:

		typedef Unique::Quaternion value_type;
		DEFINE_GET_TYPESTRING_BASICTYPE(Quaternion)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}
	};

}

#endif