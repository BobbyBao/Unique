#pragma once

namespace Unique
{
	static const uint M_MAX_UNSIGNED = 0xffffffff;

	/// Update a hash with the given 8-bit value using the SDBM algorithm.
	inline unsigned SDBMHash(unsigned hash, unsigned char c) { return c + (hash << 6) + (hash << 16) - hash; }


}