#ifndef SERIALIZETRAITS_H
#define SERIALIZETRAITS_H

#include "SerializeDefs.h"
#include "../Math/MathDefs.h"
#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include "../Math/Quaternion.h"
#include "../Math/Color.h"
#include "../Container/Str.h"

namespace Unique
{

	template<>
	class SerializeTraits<float> : public SerializeTraitsPrimitive<float>
	{
	};

	template<>
	class SerializeTraits<double> : public SerializeTraitsPrimitive<double>
	{
	};

	template<>
	class SerializeTraits<int> : public SerializeTraitsPrimitive<int>
	{
	};

	template<>
	class SerializeTraits<unsigned int> : public SerializeTraitsPrimitive<unsigned int>
	{
	};

	template<>
	class SerializeTraits<short> : public SerializeTraitsPrimitive<short>
	{
	};

	template<>
	class SerializeTraits<unsigned short> : public SerializeTraitsPrimitive<unsigned short>
	{
	};

	template<>
	class SerializeTraits<unsigned char> : public SerializeTraitsPrimitive<unsigned char>
	{
	};

	template<>
	class SerializeTraits<char> : public SerializeTraitsPrimitive<char>
	{
	};

	template<>
	class SerializeTraits<char*> : public SerializeTraitsPrimitive<char*>
	{
	};

	template<>
	class SerializeTraits<bool> : public SerializeTraitsPrimitive<bool>
	{
	};
	
	template<>
	class SerializeTraits<Unique::String> : public SerializeTraitsPrimitive<Unique::String>
	{
	public:
		static bool IsContinousMemoryArray() { return true; }
	};
	
	template<>
	class SerializeTraits<Unique::Vector2> : public SerializeTraitsBase<Unique::Vector2>
	{
	};

	template<>
	class SerializeTraits<Unique::Vector3> : public SerializeTraitsBase<Unique::Vector3>
	{
	};

	template<>
	class SerializeTraits<Unique::Vector4> : public SerializeTraitsBase<Unique::Vector4>
	{
	};

	template<>
	class SerializeTraits<Unique::Color> : public SerializeTraitsBase<Unique::Color>
	{
	};

	template<>
	class SerializeTraits<Unique::Quaternion> : public SerializeTraitsBase<Unique::Quaternion>
	{
	};

}

#endif