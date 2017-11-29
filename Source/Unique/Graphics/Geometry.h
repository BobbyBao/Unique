#pragma once
#include "../Core/Object.h"

namespace Unique
{
	class VertexBuffer;
	class IndexBuffer;
	class StorageBuffer;

	class Geometry : public Object
	{
		uRTTI(Geometry, Object)
	public:
		Geometry();
		~Geometry();

		void Draw();

		Vector<SPtr<VertexBuffer>> vertexBuffers_;
		SPtr<IndexBuffer> indexBuffer_;
	};


}
