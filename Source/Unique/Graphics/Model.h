#pragma once
#include "../Resource/Resource.h"

namespace Unique
{
	class VertexBuffer;
	class IndeexBuffer;

	class Model : public Resource
	{
		uRTTI(Model, Resource)
	public:
		Model();
		~Model();

		Vector<SPtr<VertexBuffer>> vertexBuffers_;
	//	SPtr<IndexBuffer> indexBuffer_;

	};


}
