#pragma once
#include "../Core/Object.h"

namespace Unique
{
	class Geometry;
	class Material;
	class ShaderInstance;

	class Batch
	{
	public:
		Geometry* geometry_;
		Material* material_;
		ShaderInstance*  shaderInstance_;
		void* worldTransform;

		void Draw();
	};

	class View : public Object
	{
		uRTTI(View, Object)
	public:
		View();
		~View();

		void Update();
		void Render();
	};

}

