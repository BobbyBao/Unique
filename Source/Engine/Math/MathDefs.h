#pragma once

#include "Gauss/Vector2.h"
#include "Gauss/Vector3.h"
#include "Gauss/Vector4.h"
#include "Gauss/Quaternion.h"
#include "LLGL/ColorRGBA.h"

namespace Unique
{
	using Vector2 = Gs::Vector2;
	using Vector3 = Gs::Vector3;
	using Vector4 = Gs::Vector4;
	using Quaternion = Gs::Quaternion;
	using Color = LLGL::ColorRGBA;

	static const uint M_MAX_UNSIGNED = 0xffffffff;

	/// Update a hash with the given 8-bit value using the SDBM algorithm.
	inline unsigned SDBMHash(unsigned hash, unsigned char c) { return c + (hash << 6) + (hash << 16) - hash; }


}