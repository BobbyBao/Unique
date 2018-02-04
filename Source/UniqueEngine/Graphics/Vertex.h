#pragma once


namespace Unique
{
	struct VertexPosTex
	{
		Vector3 position;
		Vector2 texCoord;

		bool operator==(const VertexPosTex& other) const {
			return position == other.position && texCoord == other.texCoord;
		}
	};

	struct VertexPosNormTex
	{
		Vector3 position;
		Vector3 normal;
		Vector2 texCoord;

		bool operator==(const VertexPosNormTex& other) const {
			return position == other.position && normal == other.normal && texCoord == other.texCoord;
		}
	};

	struct Vertex2D
	{
		Vector2 position;
		Vector2 texCoord;
		uint color;

		bool operator==(const Vertex2D& other) const {
			return position == other.position && texCoord == other.texCoord && color == other.color;
		}
	};
}


namespace std
{
	using namespace Unique;

	template<> struct hash<VertexPosTex>
	{
		size_t operator()(VertexPosTex const& vertex) const
		{
			size_t seed = 0;
			hash<Vector3> hashVec3;
			hash<Vector2> hashVec2;
			HashCombine(seed, hashVec3(vertex.position));
			HashCombine(seed, hashVec2(vertex.texCoord));
			return seed;
		}
	};

	template<> struct hash<VertexPosNormTex>
	{
		size_t operator()(VertexPosNormTex const& vertex) const
		{
			size_t seed = 0;
			hash<Vector3> hashVec3;
			hash<Vector2> hashVec2;
			HashCombine(seed, hashVec3(vertex.position));
			HashCombine(seed, hashVec3(vertex.normal));
			HashCombine(seed, hashVec2(vertex.texCoord));
			return seed;
		}
	};
}