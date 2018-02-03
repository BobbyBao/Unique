#pragma once

namespace Unique
{


}

namespace std
{	
	inline void HashCombine(size_t &seed, size_t hash)
	{
		hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hash;
	}

	template<>
	struct hash<Unique::Vector2>
	{
		size_t operator()(const Unique::Vector2& v) const
		{
			size_t seed = 0;
			hash<float> hasher;
			HashCombine(seed, hasher(v.x_));
			HashCombine(seed, hasher(v.y_));
			return seed;
		}
	};	
	
	template<>
	struct hash<Unique::Vector3>
	{
		size_t operator()(const Unique::Vector3& v) const
		{
			size_t seed = 0;
			hash<float> hasher;
			HashCombine(seed, hasher(v.x_));
			HashCombine(seed, hasher(v.y_));
			HashCombine(seed, hasher(v.z_));
			return seed;
		}
	};

	template<>
	struct hash<Unique::Vector4>
	{
		size_t operator()(const Unique::Vector4& v) const
		{
			size_t seed = 0;
			hash<float> hasher;
			HashCombine(seed, hasher(v.x_));
			HashCombine(seed, hasher(v.y_));
			HashCombine(seed, hasher(v.z_));
			HashCombine(seed, hasher(v.w_));
			return seed;
		}
	};

	template<>
	struct hash<Unique::Color>
	{
		size_t operator()(const Unique::Color& v) const
		{
			size_t seed = 0;
			hash<float> hasher;
			HashCombine(seed, hasher(v.r_));
			HashCombine(seed, hasher(v.g_));
			HashCombine(seed, hasher(v.b_));
			HashCombine(seed, hasher(v.a_));
			return seed;
		}
	};
}