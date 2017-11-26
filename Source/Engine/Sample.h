#pragma once

#include "Engine/Application.h"
#include "Graphics/Technique.h"

namespace Unique
{
	class Texture;
	class VertexBuffer;

	class Sample : public Unique::Application
	{
	public:
		Sample();

		LLGL::VertexFormat CreateBuffers();
		void CreatePipelines();
		void CreateTextures();
	private:
		virtual void Initialize();
		virtual void Terminate();
		virtual void OnDrawFrame() override;

		LLGL::ShaderProgram*    shaderProgram = nullptr;
		LLGL::GraphicsPipeline* pipeline = nullptr;
		SPtr<VertexBuffer>      vertexBuffer = nullptr;
		SPtr<Unique::Texture>   colorMap = nullptr;
		int                     samplerIndex = 0;
	};

}