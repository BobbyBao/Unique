#pragma once

#include "Engine/Application.h"
#include "Graphics/Technique.h"


class Sample : public Unique::Application
{
public:

	Sample();

	LLGL::VertexFormat CreateBuffers();
	void CreatePipelines();
	void CreateTextures();
	void CreateSamplers();
private:
	void OnDrawFrame() override;

	LLGL::ShaderProgram*    shaderProgram = nullptr;
	LLGL::GraphicsPipeline* pipeline = nullptr;
	LLGL::Buffer*           vertexBuffer = nullptr;
	LLGL::Texture*          colorMap = nullptr;
	LLGL::TextureArray*     textureArray = nullptr;
	LLGL::Sampler*          sampler[5] = { nullptr };
	int                     samplerIndex = 0;
};