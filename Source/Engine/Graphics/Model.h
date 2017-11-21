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

		// Loads the vertices with position and normal from the specified Wavefront OBJ model file.
		static Vector<VertexPositionNormal> LoadObjModel(const String& filename);

	};


}
