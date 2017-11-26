#pragma once
#include "../Core/Object.h"

namespace Unique
{
	class VertexBuffer;
	class IndeexBuffer;
	class StorageBuffer;

	class Geometry : public Object
	{
		uRTTI(Geometry, Object)
	public:
		Geometry();
		~Geometry();

		Vector<SPtr<VertexBuffer>> vertexBuffers_;
//		SPtr<IndexBuffer> indexBuffer_;
	};


}
