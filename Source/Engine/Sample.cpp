#include "Precompiled.h"
#include "Sample.h"
#include "Graphics/Technique.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/VertexBuffer.h"

using namespace Unique;


UNIQUE_IMPLEMENT_MAIN(Unique::Sample);

namespace Unique
{
	extern UPtr<LLGL::RenderSystem>        renderer;

}

Sample::Sample() :	Application { L"Unique Engine" }
{
}

void Sample::Initialize()
{
	Application::Initialize();
		// Create all graphics objects
	auto vertexFormat = CreateBuffers();

	shaderProgram = LoadStandardShaderProgram(vertexFormat);
	auto& constBuffers = shaderProgram->QueryConstantBuffers();

	CreatePipelines();
	CreateTextures();

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
	jsonWriter.Save("test.json", s);

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
	vertexBuffer = Subsystem<Graphics>().CreateVertexBuffer((uint)vertices.size(), vertexFormat, vertices.data());

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

void Sample::OnPostRender()
{
	// Examine user input
	if (input->KeyDown(LLGL::Key::Tab))
		samplerIndex = (samplerIndex + 1) % 5;

	Graphics& graphics = Subsystem<Graphics>();

	// Clear color buffer
	graphics.Clear(ClearFlags::Color);

	// Set graphics pipeline and vertex buffer
	graphics.SetGraphicsPipeline(pipeline);

	graphics.SetVertexBuffer(vertexBuffer);

	// Set texture and sampler state on slot index 0
	graphics.SetTexture(colorMap, 0);

	// Draw fullscreen triangle
	graphics.Draw(3, 0);

}

