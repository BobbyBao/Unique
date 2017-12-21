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

#define uPrimitiveTraits(type)\
	template<>\
class SerializeTraits<type> : public SerializeTraitsPrimitive<type>\
{\
};

namespace Unique
{
	uPrimitiveTraits(bool)
	uPrimitiveTraits(char)
	uPrimitiveTraits(unsigned char)	
	uPrimitiveTraits(short)
	uPrimitiveTraits(unsigned short)
	uPrimitiveTraits(int)
	uPrimitiveTraits(unsigned int)
	uPrimitiveTraits(long long)
	uPrimitiveTraits(unsigned long long)
	uPrimitiveTraits(float)
	uPrimitiveTraits(double)
	uPrimitiveTraits(char*)
	uPrimitiveTraits(String)
	uPrimitiveTraits(Vector2)
	uPrimitiveTraits(Vector3)
	uPrimitiveTraits(Vector4)
	uPrimitiveTraits(Color)
	uPrimitiveTraits(Quaternion)
}

#endif