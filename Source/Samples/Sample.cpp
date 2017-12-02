#include "Precompiled.h"
#include "Sample.h"
#include "Graphics/Shader/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/Geometry.h"

using namespace Unique;


UNIQUE_IMPLEMENT_MAIN(Unique::Sample);

namespace Unique
{
	extern UPtr<LLGL::RenderSystem>        renderer;

}

Sample::Sample() :	Application { L"Unique Engine" }
{
}

void TestIO()
{

	/*
	SPtr<Shader> shader(new Shader());
	JsonReader jsonReader;
	jsonReader.Load("Shaders/Test.shader", shader, true, true);

	JsonWriter jsonWriter;
	jsonWriter.Save("test.json", shader);

	shader->SetName("test_shader");
	SubShader* pass = shader->AddPass();
	pass->GetShaderStages().push_back({ LLGL::ShaderType::Vertex, "Assets/shader.hlsl", "VS", "vs_5_0" });
	pass->GetShaderStages().push_back({ LLGL::ShaderType::Fragment, "Assets/shader.hlsl", "PS", "ps_5_0" });

	{
		JsonWriter jsonWriter;
		jsonWriter.Save("test.json", shader);

		SPtr<Shader> s(new Shader());
		JsonReader jsonReader;
		jsonReader.Load("test.json", s);

		jsonWriter.Save("test1.json", s);
	}
	*/
	/*
	BinaryWriter ser;
	ser.Save("test.bin", shader);

	SPtr<Shader> s(new Shader());
	BinaryReader reader;
	reader.Load("test.bin", s);

	JsonWriter jsonWriter;
	jsonWriter.Save("test.json", s);*/

}

void Sample::Initialize()
{
	Application::Initialize();

	TestIO();

	SubscribeToEvent(&Sample::HandleStartup);
	SubscribeToEvent(&Sample::HandleShutdown);

	/*
	// Create all graphics objects
	auto vertexFormat = CreateBuffers();

	shaderProgram = LoadStandardShaderProgram(vertexFormat);
	auto& constBuffers = shaderProgram->QueryConstantBuffers();
	auto& uniforms = shaderProgram->QueryUniforms();

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.shaderProgram = shaderProgram;
	}

	pipeline = renderer->CreateGraphicsPipeline(pipelineDesc);
	colorMap = Texture::Load("Assets/colorMap.png");
	
	*/
}

void Sample::Terminate()
{
}

void Sample::HandleStartup(StringID, const Startup&)
{
	auto& cache = Subsystem<ResourceCache>();
	shader_ = cache.GetResource<Shader>("Shaders/Test.shader");
	shaderInst_ = shader_->GetInstance("Main", "");
	auto sp = shaderInst_->GetProgram();
	geometry_ = new Geometry();
}

void Sample::HandleShutdown(StringID, const Shutdown&)
{
}

VertexFormat Sample::CreateGeometry()
{
	// Specify vertex format
	VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({ "position", LLGL::VectorType::Float2 });
	vertexFormat.AppendAttribute({ "texCoord", LLGL::VectorType::Float2 });

	// Define vertex buffer data
	struct Vertex
	{
		Vector2 position;
		Vector2 texCoord;
	};

	Vector<Vertex> vertices =
	{
		{ { -1, -3 },{ 0, 4 } },
		{ { -1,  1 },{ 0, 0 } },
		{ { 3,  1},{ 4, 0 } },
	};

	// Create vertex buffer
	vertexBuffer = Subsystem<Graphics>().CreateVertexBuffer((uint)vertices.size(), vertexFormat, vertices.data());

	return vertexFormat;
}

void Sample::OnPostRender()
{
	// Examine user input
	if (input->KeyDown(LLGL::Key::Tab))
		samplerIndex = (samplerIndex + 1) % 5;

	Graphics& graphics = Subsystem<Graphics>();

	// Clear color buffer
	graphics.Clear(ClearFlags::Color);
	/*
	// Set graphics pipeline and vertex buffer
	graphics.SetGraphicsPipeline(pipeline);

	graphics.SetVertexBuffer(vertexBuffer);

	// Set texture and sampler state on slot index 0
	graphics.SetTexture(colorMap, 0);

	// Draw fullscreen triangle
	graphics.Draw(3, 0);*/

}

