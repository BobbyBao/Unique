#pragma once

#include "Engine/Application.h"
#include "Graphics/Shader/Shader.h"

namespace Unique
{
	class Texture;
	class VertexBuffer;

	class Sample : public Unique::Application
	{
	public:
		Sample();

		VertexFormat CreateBuffers();
	private:
		virtual void Initialize();
		virtual void Terminate();
		virtual void OnPostRender() override;

		ShaderProgram*    shaderProgram = nullptr;
		LLGL::GraphicsPipeline* pipeline = nullptr;
		SPtr<VertexBuffer>      vertexBuffer = nullptr;
		SPtr<Unique::Texture>   colorMap = nullptr;
		int                     samplerIndex = 0;
	};

}