#include "Precompiled.h"
#include "Sample.h"
#include "Graphics/Technique.h"
#include "Graphics/Texture.h"

using namespace Unique;



Sample::Sample() :
	Application { L"Unique Engine" }
{
}

void Sample::Initialize()
{
	Application::Initialize();

	// Check if samplers are supported
	const auto& renderCaps = renderer->GetRenderingCaps();

	if (!renderCaps.hasSamplers)
		UNIQUE_LOGERROR("samplers are not supported by this renderer");

	// Create all graphics objects
	auto vertexFormat = CreateBuffers();

	shaderProgram = LoadStandardShaderProgram(vertexFormat);
	auto& constBuffers = shaderProgram->QueryConstantBuffers();

	CreatePipelines();
	CreateTextures();
	CreateSamplers();

	// Print some information on the standard output
	std::cout << "press TAB KEY to switch between five different texture samplers" << std::endl;
	/*
	SPtr<Technique> shader(new Technique());
	shader->SetName("test_shader");
	Pass* pass = shader->AddPass();
	pass->GetShaderStages().push_back({ LLGL::ShaderType::Vertex, "Assets/shader.hlsl", "VS", "vs_5_0" });
	pass->GetShaderStages().push_back({ LLGL::ShaderType::Fragment, "Assets/shader.hlsl", "PS", "ps_5_0" });

	BinaryWriter ser;
	ser.Save("test.bin", shader);

	SPtr<Technique> s(new Technique());
	BinaryReader reader;
	reader.Load("test.bin", s);

	JsonWriter jsonWriter;
	jsonWriter.Save("test.json", s);*/

}

void Sample::Terminate()
{
}

LLGL::VertexFormat Sample::CreateBuffers()
{
	// Specify vertex format
	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({ "position", LLGL::VectorType::Float2 });
	vertexFormat.AppendAttribute({ "texCoord", LLGL::VectorType::Float2 });

	// Define vertex buffer data
	struct Vertex
	{
		Vector2 position;
		Vector2 texCoord;
	};

	std::vector<Vertex> vertices =
	{
		{ { -1, -3 },{ 0, 4 } },
		{ { -1,  1 },{ 0, 0 } },
		{ { 3,  1 },{ 4, 0 } },
	};

	// Create vertex buffer
	vertexBuffer = CreateVertexBuffer(vertices, vertexFormat);

	return vertexFormat;
}

void Sample::CreatePipelines()
{
	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.shaderProgram = shaderProgram;
	}
	pipeline = renderer->CreateGraphicsPipeline(pipelineDesc);
}

void Sample::CreateTextures()
{
	String texFilename = "Assets/colorMap.png";
	colorMap = Unique::Texture::Load(texFilename);
}

void Sample::CreateSamplers()
{
	// Create 1st sampler state with default settings
	LLGL::SamplerDescriptor samplerDesc;
	sampler[0] = renderer->CreateSampler(samplerDesc);

	// Create 2nd sampler state with MIP-map bias
	samplerDesc.mipMapLODBias = 3.0f;
	sampler[1] = renderer->CreateSampler(samplerDesc);

	// Create 3rd sampler state with nearest filtering
	samplerDesc.minFilter = LLGL::TextureFilter::Nearest;
	sampler[2] = renderer->CreateSampler(samplerDesc);

	// Create 4th sampler state with clamped texture wrap mode
	samplerDesc.minFilter = LLGL::TextureFilter::Linear;
	samplerDesc.mipMapLODBias = 0.0f;
	samplerDesc.textureWrapU = LLGL::TextureWrap::Clamp;
	sampler[3] = renderer->CreateSampler(samplerDesc);

	// Create 5th sampler state with mirrored texture wrap mode
	samplerDesc.textureWrapU = LLGL::TextureWrap::Mirror;
	samplerDesc.textureWrapV = LLGL::TextureWrap::Mirror;
	sampler[4] = renderer->CreateSampler(samplerDesc);
}
	
void Sample::OnDrawFrame()
{
	// Examine user input
	if (input->KeyDown(LLGL::Key::Tab))
		samplerIndex = (samplerIndex + 1) % 5;

	// Clear color buffer
	commands->Clear(LLGL::ClearFlags::Color);

	// Set graphics pipeline and vertex buffer
	commands->SetGraphicsPipeline(*pipeline);
	commands->SetVertexBuffer(*vertexBuffer);

	// Set texture and sampler state on slot index 0
	commands->SetTexture(*colorMap->handle_, 0);
	commands->SetSampler(*sampler[samplerIndex], 0);

	// Draw fullscreen triangle
	commands->Draw(3, 0);

	// Present result on the screen
	graphicsContext->Present();
}

