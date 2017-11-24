#include "Precompiled.h"
#include "Sample.h"
#include "Graphics/Technique.h"

using namespace Unique;



Sample::Sample() :
	Application { L"Unique Engine" }
{
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

LLGL::VertexFormat Sample::CreateBuffers()
{
	// Specify vertex format
	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({ "position", LLGL::VectorType::Float2 });
	vertexFormat.AppendAttribute({ "texCoord", LLGL::VectorType::Float2 });

	// Define vertex buffer data
	struct Vertex
	{
		Gs::Vector2f position;
		Gs::Vector2f texCoord;
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
	std::string texFilename = "Assets/colorMap.png";

	// Load image data from file (using STBI library, see http://nothings.org/stb_image.h)
	int texWidth = 0, texHeight = 0, texComponents = 0;

	unsigned char* imageBuffer = Image::LoadImage(texFilename.c_str(), &texWidth, &texHeight, &texComponents, 0);
	if (!imageBuffer)
		throw std::runtime_error("failed to open file: \"" + texFilename + "\"");

	// Initialize image descriptor to upload image data onto hardware texture
	LLGL::ImageDescriptor imageDesc;
	{
		// Set image buffer color format
		imageDesc.format = (texComponents == 4 ? LLGL::ImageFormat::RGBA : LLGL::ImageFormat::RGB);

		// Set image buffer data type (unsigned char = 8-bit unsigned integer)
		imageDesc.dataType = LLGL::DataType::UInt8;

		// Set image buffer source for texture initial data
		imageDesc.buffer = imageBuffer;
	}

	// Upload image data onto hardware texture and stop the time
	timer->Start();
	{
		// Create texture
		LLGL::TextureDescriptor texDesc;
		{
			// Texture type: 2D
			texDesc.type = LLGL::TextureType::Texture2D;

			// Texture hardware format: RGBA with normalized 8-bit unsigned char type
			texDesc.format = LLGL::TextureFormat::RGBA;

			// Texture size
			texDesc.texture2D.width = texWidth;
			texDesc.texture2D.height = texHeight;
		}
		colorMap = renderer->CreateTexture(texDesc, &imageDesc);
	}
	auto texCreationTime = timer->Stop();
	std::cout << "texture creation time: " << texCreationTime << " microseconds" << std::endl;

	// Generate all MIP-maps (MIP = "Multum in Parvo", or "a multitude in a small space")
	// see https://developer.valvesoftware.com/wiki/MIP_Mapping
	// see http://whatis.techtarget.com/definition/MIP-map
	renderer->GenerateMips(*colorMap);

	// Release image data
	Image::FreeImage(imageBuffer);

	// Query texture descriptor to see what is really stored on the GPU
	//auto textureDesc = renderer->QueryTextureDescriptor(*colorMap);

	// Create array of textures (not to be confused with an "array texture" which is a texture of arrays)
	textureArray = renderer->CreateTextureArray(1, &colorMap);
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
	commands->SetTextureArray(*textureArray, 0);
	commands->SetSampler(*sampler[samplerIndex], 0);

	// Draw fullscreen triangle
	commands->Draw(3, 0);

	// Present result on the screen
	graphicsContext->Present();
}

