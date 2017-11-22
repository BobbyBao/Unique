#pragma once
#include "../Resource/Resource.h"

namespace Unique
{
	struct VertexPositionNormal
	{
		Gs::Vector3f position;
		Gs::Vector3f normal;
	};

	class Model : public Resource
	{
		uRTTI(Model, Resource)
	public:
		Model();
		~Model();

	};


}
