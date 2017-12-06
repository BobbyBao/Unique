#pragma once

#include "Engine/Application.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/View.h"

namespace Unique
{
	class Shader;
	class Geometry;
	class Texture;
	class VertexBuffer;

	class Sample : public Application
	{
	public:
		Sample();

	private:
		virtual void Initialize();
		virtual void Terminate();
		void HandleStartup(StringID, const struct Startup&);
		void HandleShutdown(StringID, const struct Shutdown&);
		void HandleRenderUpdate(StringID, const struct RenderUpdate&);
		virtual void OnPostRender() override;
		void CreateGeometry();

		SPtr<Shader> shader_;
		ShaderInstance* shaderInst_;
		SPtr<Geometry> geometry_;

		Vector<Batch> batches_[2];
		SPtr<Unique::Texture>   colorMap = nullptr;
	};

}