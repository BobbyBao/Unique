#pragma once

#include "Engine/Application.h"
#include "Graphics/Technique.h"

namespace Unique
{
	class Texture;

	class Sample : public Unique::Application
	{
	public:

		Sample();

		LLGL::VertexFormat CreateBuffers();
		void CreatePipelines();
		void CreateTextures();
		void CreateSamplers();
	private:
		virtual void Initialize();
		virtual void Terminate();
		virtual void OnDrawFrame() override;

		LLGL::ShaderProgram*    shaderProgram = nullptr;
		LLGL::GraphicsPipeline* pipeline = nullptr;
		LLGL::Buffer*           vertexBuffer = nullptr;
		SPtr<Unique::Texture>   colorMap = nullptr;
		LLGL::Sampler*          sampler[5] = { nullptr };
		int                     samplerIndex = 0;
	};

}