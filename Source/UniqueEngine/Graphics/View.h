#pragma once
#include "Core/Object.h"

namespace Unique
{
	class Geometry;
	class Material;
	class PipelineState;

	struct ViewParameter
	{
		Matrix4 view_;
		Matrix4 viewInv_;
		Matrix4 viewProj_;
	};

	struct ObjectParameter
	{
		Matrix4 world_;
	};

	class Batch
	{
	public:
		Geometry* geometry_;
		Material* material_;
		PipelineState*  pipelineState_;
		void* worldTransform_;
		uint numWorldTransform_;

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

