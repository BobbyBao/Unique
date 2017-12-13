#pragma once
#include "Core/Object.h"

namespace Unique
{
	class Geometry;
	class Material;
	class PipelineState;

	class Batch
	{
	public:
		Geometry* geometry_;
		Material* material_;
		PipelineState*  pipelineState_;
		void* worldTransform_;
		uint numWorldTransforms_;

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

