#include "UniquePCH.h"
#include <memory>
#include <iomanip>
#include "UniqueSample.h"
#include "MapHelper.h"
#include "Graphics/Geometry.h"
#include "Graphics/GraphicsBuffer.h"
#include <BasicShaderSourceStreamFactory.h>
#include "Graphics/Shader.h"

UNIQUE_IMPLEMENT_MAIN(Unique::UniqueSample)

using namespace Diligent;

namespace Unique
{
	struct Vertex
	{
		float3 Position;
		float3 Normal;
		unsigned int AmbientColor;
	};
	struct ShaderConstants
	{
		float4x4 WorldViewProjT;
		float4x4 WorldNormT;
		float3 LightDir;
		float LightCoeff;
	};

	// Each cube face is split into a 3x3 grid
	const int CUBE_FACE_VERTEX_COUNT = 4 * 4;       // 16 vertices per face
	const int CUBE_FACE_TRIANGLE_COUNT = 2 * 3 * 3; // 18 triangles to be drawn for each face
													// Faces color of the sponge wrt to recursion level
	const unsigned int COLORS[] = { 0xffffffff, 0xff007fff, 0xff7fff00, 0xffff007f, 0xff0000ff, 0xff00ff00, 0xffff0000 };

	UniqueSample::UniqueSample()
	{
		m_SpongeLevel = 2;                       // number of recursions
		m_SpongeAO = true;                      // apply ambient occlusion
		m_LightDir[0] = -0.5f;
		m_LightDir[1] = -0.2f;
		m_LightDir[2] = 1;
		m_CamDistance = 0.7f;                  // camera distance
		m_BackgroundColor[0] = 0;
		m_BackgroundColor[1] = 0;
		m_BackgroundColor[2] = 0.5f;
		m_BackgroundColor[3] = 1;
		m_Animate = false;                       // enable animation
		m_AnimationSpeed = 0.2f;               // animation speed

		SetDeviceType(DeviceType::D3D11);
	}

	UniqueSample::~UniqueSample()
	{
	}

	void UniqueSample::Initialize()
	{
		Application::Initialize();

		CreateResource();

		SPtr<Shader> shader(new Shader());
		JsonReader jsonReader;
		jsonReader.Load("Shaders/Test.shader", shader, true, true);

		JsonWriter jsonWriter;
		jsonWriter.Save("test2.json", shader);
		
		/*
		{
			shader->SetName("test_shader");
			Pass* pass = shader->AddPass("Test");
			pass->shaderStage_[0] = { ShaderType::SHADER_TYPE_VERTEX, "Assets/shader.hlsl", "VS", "vs_5_0" };
			pass->shaderStage_[1] = { ShaderType::SHADER_TYPE_PIXEL, "Assets/shader.hlsl", "PS", "ps_5_0" };

			JsonWriter jsonWriter;
			jsonWriter.Save("test.json", shader);

			SPtr<Shader> s(new Shader());
			JsonReader jsonReader;
			jsonReader.Load("test.json", s);

			jsonWriter.Save("test1.json", s);
		}*/
	}

	void UniqueSample::Terminate()
	{
	}


	void UniqueSample::CreateResource()
	{

		auto& graphics = GetSubsystem<Graphics>();

		BufferDesc BuffDesc;
		BuffDesc.BindFlags = BIND_UNIFORM_BUFFER;
		BuffDesc.Usage = USAGE_DYNAMIC;
		BuffDesc.uiSizeInBytes = sizeof(ShaderConstants);
		BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
		renderDevice->CreateBuffer(BuffDesc, BufferData(), &m_pConstantBuffer);

		// Create vertex and index buffers
		BuildSponge(m_SpongeLevel, m_SpongeAO);

		CreatePipeline();

		// Init model rotation
		float3 axis(-1, 1, 0);
		m_SpongeRotation = RotationFromAxisAngle(axis, M_PI / 4);
	}

	void UniqueSample::CreatePipeline()
	{
		{
			ShaderCreationAttribs Attrs;
			//Attrs.Desc.Name = "MainVS";
			Attrs.FilePath = "MainVS_DX.hlsl";
			//Attrs.EntryPoint = "main";
			Attrs.Desc.ShaderType = SHADER_TYPE_VERTEX;
			Attrs.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
			//Attrs.Desc.TargetProfile = SHADER_PROFILE_DX_4_0;
			BasicShaderSourceStreamFactory BasicSSSFactory("assets\\shaders;assets\\shaders\\inc;");
			Attrs.pShaderSourceStreamFactory = &BasicSSSFactory;
			renderDevice->CreateShader(Attrs, &vs_);
		}

		{
			ShaderCreationAttribs Attrs;
			//Attrs.Desc.Name = "MainPS";
			Attrs.FilePath = "MainPS_DX.hlsl";
			//Attrs.EntryPoint = "main";
			Attrs.Desc.ShaderType = SHADER_TYPE_PIXEL;
			Attrs.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
			//Attrs.Desc.TargetProfile = SHADER_PROFILE_DX_4_0;
			BasicShaderSourceStreamFactory BasicSSSFactory("assets\\shaders;assets\\shaders\\inc;");
			Attrs.pShaderSourceStreamFactory = &BasicSSSFactory;
			renderDevice->CreateShader(Attrs, &ps_);
		}

		{

			PipelineStateDesc PSODesc;
			PSODesc.IsComputePipeline = false;
			PSODesc.GraphicsPipeline.NumRenderTargets = 1;
			PSODesc.GraphicsPipeline.RTVFormats[0] = TEX_FORMAT_RGBA8_UNORM_SRGB;
			PSODesc.GraphicsPipeline.DSVFormat = TEX_FORMAT_D32_FLOAT;

			// Init depth-stencil state
			DepthStencilStateDesc &DepthStencilDesc = PSODesc.GraphicsPipeline.DepthStencilDesc;
			DepthStencilDesc.DepthEnable = true;
			DepthStencilDesc.DepthWriteEnable = true;

			// Init blend state
			BlendStateDesc &BSDesc = PSODesc.GraphicsPipeline.BlendDesc;
			BSDesc.IndependentBlendEnable = False;
			auto &RT0 = BSDesc.RenderTargets[0];
			RT0.BlendEnable = False;
			/*
			RT0.RenderTargetWriteMask = COLOR_MASK_ALL;
			RT0.SrcBlend = BLEND_FACTOR_SRC_ALPHA;
			RT0.DestBlend = BLEND_FACTOR_INV_SRC_ALPHA;
			RT0.BlendOp = BLEND_OPERATION_ADD;
			RT0.SrcBlendAlpha = BLEND_FACTOR_SRC_ALPHA;
			RT0.DestBlendAlpha = BLEND_FACTOR_INV_SRC_ALPHA;
			RT0.BlendOpAlpha = BLEND_OPERATION_ADD;*/

			// Init rasterizer state
			RasterizerStateDesc &RasterizerDesc = PSODesc.GraphicsPipeline.RasterizerDesc;
			RasterizerDesc.FillMode = FILL_MODE_SOLID;
			RasterizerDesc.CullMode = CULL_MODE_NONE;
			//RasterizerDesc.FrontCounterClockwise = True;
			RasterizerDesc.ScissorEnable = False;
			//RSDesc.MultisampleEnable = false; // do not allow msaa (fonts would be degraded)
			RasterizerDesc.AntialiasedLineEnable = False;

			InputLayoutDesc &Layout = PSODesc.GraphicsPipeline.InputLayout;
			LayoutElement TextLayoutElems[] =
			{
				LayoutElement(0, 0, 3, VT_FLOAT32, False),
				LayoutElement(1, 0, 3, VT_FLOAT32, False),
				LayoutElement(2, 0, 4, VT_UINT8, True),
			};
			Layout.LayoutElements = TextLayoutElems;
			Layout.NumElements = _countof(TextLayoutElems);

			PSODesc.GraphicsPipeline.PrimitiveTopologyType = PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			PSODesc.GraphicsPipeline.pVS = vs_;
			PSODesc.GraphicsPipeline.pPS = ps_;

			renderDevice->CreatePipelineState(PSODesc, &pipelineState_);
			pipelineState_->CreateShaderResourceBinding(&pSRB_);
		}


		ResourceMappingEntry entries[] =
		{
			{ "Constants", m_pConstantBuffer },

			ResourceMappingEntry()
		};

		ResourceMappingDesc rmd;
		rmd.pEntries = entries;

		renderDevice->CreateResourceMapping(rmd, &resourceMapping_);
		pipelineState_->BindShaderResources(resourceMapping_, BIND_SHADER_RESOURCES_ALL_RESOLVED);

	}
	
	void AppendCubeToBuffers(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const float4x4& xform, float aoRatio, const bool aoEdges[12],
		const unsigned int faceColors[6]);

	void FillSpongeBuffers(int level, int levelMax, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const float3& center, bool aoEnabled, const bool aoEdges[12], const unsigned int faceColors[6]);
	
	// Build sponge vertex and index buffers 
	void UniqueSample::BuildSponge(int levelMax, bool aoEnabled)
	{
		// Fill vertex and index memory buffers
		static std::vector<Vertex> vertices;
		static std::vector<unsigned int> indices;
		vertices.clear();
		indices.clear();
		bool aoEdges[12] = { false, false, false, false, false, false, false, false, false, false, false, false };
		unsigned int faceColors[6] = { COLORS[0], COLORS[0], COLORS[0], COLORS[0], COLORS[0], COLORS[0] };
		FillSpongeBuffers(0, levelMax, vertices, indices, float3(), aoEnabled, aoEdges, faceColors);

		SPtr<VertexBuffer> pVertexBuffer(new VertexBuffer());
		pVertexBuffer->Create(vertices);

		SPtr<IndexBuffer> pIndexBuffer(new IndexBuffer());
		pIndexBuffer->Create(indices);

		geometry_ = new Geometry();
		geometry_->SetNumVertexBuffers(1);
		geometry_->SetVertexBuffer(0, pVertexBuffer);
		geometry_->SetIndexBuffer(pIndexBuffer);
		geometry_->SetDrawRange(PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 
			0, (unsigned int)indices.size());
	}

	// Copy world/view/proj matrices and light parameters to shader constants
	void UniqueSample::SetShaderConstants(const float4x4& world, const float4x4& view, const float4x4& proj)
	{
		MapHelper<ShaderConstants> MappedData(deviceContext, m_pConstantBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
		ShaderConstants *cst = MappedData;
		cst->WorldViewProjT = transposeMatrix(world * view * proj);
		cst->WorldNormT = transposeMatrix(world);
		cst->LightDir = (1.0f / length(m_LightDir)) * m_LightDir;
		cst->LightCoeff = 0.85f;
	}

	// Append vertices and indices of a cube to the index and vertex buffers.
	// The cube has gradient ambient-occlusion defined per edge.
	void AppendCubeToBuffers(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const float4x4& xform, float aoRatio, const bool aoEdges[12],
		const unsigned int faceColors[6])
	{
		unsigned int indicesOffset = (unsigned int)vertices.size();

		// Fill vertex buffer

		// Cube faces and edges numbering:
		//       __________           _____6____ 
		//      /         /|         /|        /|
		//     /    4    / |<2     10 5       9 |
		//    /_________/  |       /__|__2___/  7
		//    |         | 1|       |  |___4__|__|
		//  3>|    0    |  /       3  /      1  /
		//    |         | /        | 11      | 8 
		//    |_________|/         |/____0___|/  
		//         5^
		// Each face is split in a 3x3 grid, which gives 16 vertices per face and 3x3x2(=18) triangles per face.
		// Ambient occlusion color is set for each of these vertices wrt aoEdges flags.

		const float R = 0.5f; // unit cube radius
							  // the 4 corner coordinates for each of the 6 faces
		const float3 A[6] = { float3(-R, -R, -R), float3(+R, -R, -R), float3(+R, -R, +R), float3(-R, -R, +R), float3(-R, +R, -R), float3(-R, -R, -R) };
		const float3 B[6] = { float3(+R, -R, -R), float3(+R, -R, +R), float3(-R, -R, +R), float3(-R, -R, -R), float3(+R, +R, -R), float3(+R, -R, -R) };
		const float3 C[6] = { float3(-R, +R, -R), float3(+R, +R, -R), float3(+R, +R, +R), float3(-R, +R, +R), float3(-R, +R, +R), float3(-R, -R, +R) };
		const float3 D[6] = { float3(+R, +R, -R), float3(+R, +R, +R), float3(-R, +R, +R), float3(-R, +R, -R), float3(+R, +R, +R), float3(+R, -R, +R) };
		// the 6 face normals
		const float3 N[6] = { float3(0,  0, -1), float3(+1,  0,  0), float3(0,  0, +1), float3(-1,  0,  0), float3(0, +1,  0), float3(0, -1,  0) };
		// association between edge indices and the 6 faces
		const int E[6][4] = { { 0, 1, 2, 3 },{ 8, 7, 9, 1 },{ 4, 5, 6, 7 },{ 11, 3, 10, 5 },{ 2, 9, 6, 10 },{ 0, 8, 4, 11 } };

		int face, i, j;
		float u, v;
		bool ao;
		Vertex vertex;
		for (face = 0; face < 6; face++)
			for (j = 0; j < 4; j++)
			{
				v = (j == 1) ? aoRatio : ((j == 2) ? 1.0f - aoRatio : j / 3);
				for (i = 0; i < 4; i++)
				{
					u = (i == 1) ? aoRatio : ((i == 2) ? 1.0f - aoRatio : i / 3);

					vertex.Position = (1.0f - v) * ((1.0f - u) * A[face] + u * B[face])
						+ v * ((1.0f - u) * C[face] + u * D[face]);
					vertex.Position = vertex.Position * xform;

					vertex.Normal = N[face];

					ao = (j == 0) && aoEdges[E[face][0]];
					ao |= (i == 3) && aoEdges[E[face][1]];
					ao |= (j == 3) && aoEdges[E[face][2]];
					ao |= (i == 0) && aoEdges[E[face][3]];

#define DARKEN(r, s) ( (unsigned int)(float(r)*(s)) > 255 ? 255 : (unsigned int)(float(r)*(s)) )
#define DARKEN_COLOR(c, s) ( 0xff000000 | (DARKEN(((c)>>16)&0xff, s)<<16) | (DARKEN(((c)>>8)&0xff, s)<<8) | DARKEN((c)&0xff, s) )
					vertex.AmbientColor = ao ? DARKEN_COLOR(faceColors[face], 0.75f) : faceColors[face];

					vertices.push_back(vertex);
				}
			}

		// Fill index buffer

		// 3 indices per triangle, 2*3*3 triangles per faces, 6 faces.
		// Vertex index numbering of each face:
		//    12__13__14___15
		//     |'. | .'| .'|
		//     8__'9'_10'__11
		//     | .'| .'| .'|
		//     4'__5'__6'__7
		//     | .'| .'|'. |
		//     0'__1'__2__'3

		const unsigned short I[CUBE_FACE_TRIANGLE_COUNT][3] =
		{
			{ 0, 5, 4 },{ 0, 1, 5 },{ 1, 6, 5 },{ 1, 2, 6 },{ 3, 6, 2 },{ 3, 7, 6 },
			{ 4, 9, 8 },{ 4, 5, 9 },{ 5, 10, 9 },{ 5, 6, 10 },{ 6, 11, 10 },{ 6, 7, 11 },
			{ 8, 9, 12 },{ 9, 13, 12 },{ 9, 14, 13 },{ 9, 10, 14 },{ 10, 15, 14 },{ 10, 11, 15 }
		};
		int tri;
		for (face = 0; face < 6; face++)
			for (tri = 0; tri < CUBE_FACE_TRIANGLE_COUNT; tri++)
				for (i = 0; i < 3; i++)
					indices.push_back(indicesOffset + I[tri][i] + 16 * face); // 16 vertices per face
	}


	// Recursive function called to fill the vertex and index buffers with the cubes forming the Menger sponge.
	void FillSpongeBuffers(int level, int levelMax, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const float3& center, bool aoEnabled, const bool aoEdges[12], const unsigned int faceColors[6])
	{
		float scale = pow(1.0f / 3.0f, level);

		if (level == levelMax)
		{
			float aoRatio = pow(3.0f, level) * 0.02f;
			if (aoRatio > 0.4999f)
				aoRatio = 0.4999f;
			float4x4 xform = scaleMatrix(scale, scale, scale) * translationMatrix(center);
			AppendCubeToBuffers(vertices, indices, xform, aoRatio, aoEdges, faceColors);
		}
		else
		{
			// Local function which applies AO in one direction
			struct Local
			{
				static void ApplyAO(int i, int j, bool& e0, bool& e1, bool& e2, bool& e3)
				{
					if (i == -1 && j == 0) e0 = e1 = true;
					if (i == +1 && j <= 0) e1 = false;
					if (i == +1 && j >= 0) e0 = false;

					if (i == +1 && j == 0) e2 = e3 = true;
					if (i == -1 && j <= 0) e2 = false;
					if (i == -1 && j >= 0) e3 = false;

					if (j == -1 && i == 0) e1 = e2 = true;
					if (j == +1 && i <= 0) e1 = false;
					if (j == +1 && i >= 0) e2 = false;

					if (j == +1 && i == 0) e0 = e3 = true;
					if (j == -1 && i <= 0) e0 = false;
					if (j == -1 && i >= 0) e3 = false;
				}
			};

			bool aoEdgesCopy[12];
			unsigned int faceColorsCopy[6];
			int i, j, k, l;
			for (i = -1; i <= 1; i++)
				for (j = -1; j <= 1; j++)
					for (k = -1; k <= 1; k++)
						if (!((i == 0 && j == 0) || (i == 0 && k == 0) || (j == 0 && k == 0)))
						{
							float s = 1.0f / 3.0f * scale;
							float3 t(center[0] + s * i, center[1] + s * j, center[2] + s * k);

							for (l = 0; l < 12; l++)
								aoEdgesCopy[l] = aoEdges[l];
							if (aoEnabled)
							{
								Local::ApplyAO(i, j, aoEdgesCopy[8], aoEdgesCopy[9], aoEdgesCopy[10], aoEdgesCopy[11]); // z direction
								Local::ApplyAO(i, k, aoEdgesCopy[1], aoEdgesCopy[7], aoEdgesCopy[5], aoEdgesCopy[3]); // y direction
								Local::ApplyAO(-k, j, aoEdgesCopy[0], aoEdgesCopy[2], aoEdgesCopy[6], aoEdgesCopy[4]); // x direction
							}

							for (l = 0; l < 6; l++)
								faceColorsCopy[l] = faceColors[l];
							if (k == +1) faceColorsCopy[0] = COLORS[level + 1];
							if (i == -1) faceColorsCopy[1] = COLORS[level + 1];
							if (k == -1) faceColorsCopy[2] = COLORS[level + 1];
							if (i == +1) faceColorsCopy[3] = COLORS[level + 1];
							if (j == -1) faceColorsCopy[4] = COLORS[level + 1];
							if (j == +1) faceColorsCopy[5] = COLORS[level + 1];

							FillSpongeBuffers(level + 1, levelMax, vertices, indices, t, aoEnabled, aoEdgesCopy, faceColorsCopy);
						}
		}
	}
	
	void UniqueSample::OnPreRender()
	{
		float dt = (float)0.05f;
		if (m_Animate && dt > 0 && dt < 0.2f)
		{
			float3 axis;
			float angle = 0;
			AxisAngleFromRotation(axis, angle, m_SpongeRotation);
			if (length(axis) < 1.0e-6f)
				axis[1] = 1;
			angle += m_AnimationSpeed * dt;
			if (angle >= 2.0f*M_PI)
				angle -= 2.0f*M_PI;
			else if (angle <= 0)
				angle += 2.0f*M_PI;
			m_SpongeRotation = RotationFromAxisAngle(axis, angle);
		}

		auto& graphics = GetSubsystem<Graphics>();
		// Clear the back buffer 
		deviceContext->ClearRenderTarget(nullptr, m_BackgroundColor);
		deviceContext->ClearDepthStencil(nullptr, CLEAR_DEPTH_FLAG, 1.f);

		// Set world/view/proj matrices and global shader constants
		float aspectRatio = graphics.GetAspectRatio();
		float4x4 proj = Projection(M_PI / 4, aspectRatio, 0.1f, 100.0f, graphics.IsDirect3D());
		float dist = m_CamDistance + 0.4f;
		float3 camPosInv(dist * 0.3f, dist * 0.0f, dist * 2.0f);
		float4x4 view = translationMatrix(camPosInv);
		float4x4 world = QuaternionToMatrix(m_SpongeRotation);

		SetShaderConstants(world, view, proj);

		deviceContext->SetPipelineState(pipelineState_);
		pSRB_->BindResources(SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL, resourceMapping_,
			BIND_SHADER_RESOURCES_UPDATE_UNRESOLVED | BIND_SHADER_RESOURCES_ALL_RESOLVED);
		deviceContext->CommitShaderResources(pSRB_, COMMIT_SHADER_RESOURCES_FLAG_TRANSITION_RESOURCES);
		
		geometry_->Draw(pipelineState_);

	}

	void UniqueSample::OnPostRender()
	{
	}


}
