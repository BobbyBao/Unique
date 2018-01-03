#include "UniquePCH.h"
#include "CubeSample.h"
#include "Graphics/Shader.h"
#include "Graphics/PipelineState.h"
#include "MapHelper.h"
#include "Graphics/Geometry.h"
#include "Graphics/VertexBuffer.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"

//UNIQUE_IMPLEMENT_MAIN(Unique::CubeSample)

namespace Unique
{

	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		uint AmbientColor;
	};

	struct ShaderConstants
	{
		Matrix4 WorldViewProjT;
		Matrix4 WorldNormT;
		Vector3 LightDir;
		float LightCoeff;
	};

	SPtr<Geometry> BuildSponge(int levelMax, bool aoEnabled);
	void AppendCubeToBuffers(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const Matrix3x4& xform, float aoRatio, const bool aoEdges[12],
		const unsigned int faceColors[6]);

	void FillSpongeBuffers(int level, int levelMax, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const Vector3& center, bool aoEnabled, const bool aoEdges[12], const unsigned int faceColors[6]);

	CubeSample::CubeSample()
		: backgroundColor_(0.0f, 0.0f, 0.5f, 1.0f)
	{
		spongeRotation_ = Quaternion::IDENTITY;

		m_SpongeLevel = 2;                      // number of recursions
		m_SpongeAO = true;                      // apply ambient occlusion
		m_LightDir.x_ = -0.5f;
		m_LightDir.y_ = -0.2f;
		m_LightDir.z_ = 1;
		m_CamDistance = 0.7f;                  // camera distance
		m_AnimationSpeed = 0.02f;               // animation speed

		Subscribe(&CubeSample::HandleStartup);
		Subscribe(&CubeSample::HandleShutdown);
		Subscribe(&CubeSample::HandleUpdate);
		Subscribe(&CubeSample::HandleRenderUpdate);
	}


	CubeSample::~CubeSample()
	{
	}


	void CubeSample::HandleStartup(const struct Startup& eventData)
	{
		auto& cache = GetSubsystem<ResourceCache>();
		auto& graphics = GetSubsystem<Graphics>();

		constBuffer_ = new UniformBuffer(ShaderConstants());

		// Create vertex and index buffers
		geometry_ = BuildSponge(m_SpongeLevel, m_SpongeAO);

		shader_ = cache.GetResource<Shader>("Shaders/Test.shader");
		pipeline_ = shader_->GetPipeline("base", "");
		graphics.AddResource("Constants", constBuffer_);
	//	graphics.Frame();	
		
	}

	void CubeSample::HandleShutdown(const struct Shutdown& eventData)
	{
		UNIQUE_LOGINFO("Shutdown");
	}

	void CubeSample::HandleUpdate(const struct Update& eventData)
	{
		float dt = (float)eventData.timeStep_;
		
		if (m_Animate && dt > 0 && dt < 0.2f)
		{
			yaw_ += m_AnimationSpeed * dt;
			spongeRotation_ = Quaternion(0, yaw_, 0.0f);
		}

		auto& graphics = GetSubsystem<Graphics>();
		float aspectRatio = graphics.GetAspectRatio();

		Matrix4 proj = Matrix4::CreateProjection(M_PI / 4, aspectRatio, 0.1f, 100.0f, graphics.IsDirect3D());

		float dist = m_CamDistance + 0.4f;
		Vector3 camPosInv(dist * 0.3f, dist * 0.0f, dist * 2.0f);

		Matrix4 view = Matrix4::IDENTITY;
		view.SetTranslation(camPosInv);

		Matrix3x4 world(Vector3::ZERO, spongeRotation_, Vector3::ONE);

		ShaderConstants *cst = (ShaderConstants *)constBuffer_->Lock();
		cst->WorldViewProjT = (proj* view *world);
		cst->WorldNormT = world.ToMatrix4();
		cst->LightDir = (1.0f / m_LightDir.Length()) * m_LightDir;
		cst->LightCoeff = 0.85f;
		constBuffer_->Unlock();
	}

	void CubeSample::HandleRenderUpdate(const struct RenderUpdate& eventData)
	{
		Graphics& graphics = GetSubsystem<Graphics>();
		Vector<Batch>& batches = geometries_[graphics.currentContext_];
		batches.clear();
		Batch batch;
		batch.geometry_ = geometry_;
		batch.pipelineState_ = pipeline_;
		batches.emplace_back(batch);
	}
	
	void CubeSample::OnPreRender()
	{
		auto& graphics = GetSubsystem<Graphics>();
		// Clear the back buffer 
		deviceContext->ClearRenderTarget(nullptr, backgroundColor_);
		deviceContext->ClearDepthStencil(nullptr, Diligent::CLEAR_DEPTH_FLAG, 1.f);

		Vector<Batch>& batches = geometries_[graphics.GetRenderContext()];
		for (auto& batch : batches)
		{
			batch.geometry_->Draw(pipeline_);
		}
	}


	// Each cube face is split into a 3x3 grid
	const int CUBE_FACE_VERTEX_COUNT = 4 * 4;       // 16 vertices per face
	const int CUBE_FACE_TRIANGLE_COUNT = 2 * 3 * 3; // 18 triangles to be drawn for each face
													// Faces color of the sponge wrt to recursion level
	const unsigned int COLORS[] = { 0xffffffff, 0xff007fff, 0xff7fff00, 0xffff007f, 0xff0000ff, 0xff00ff00, 0xffff0000 };

	// Build sponge vertex and index buffers 
	SPtr<Geometry> BuildSponge(int levelMax, bool aoEnabled)
	{
		// Fill vertex and index memory buffers
		static std::vector<Vertex> vertices;
		static std::vector<unsigned int> indices;
		vertices.clear();
		indices.clear();
		bool aoEdges[12] = { false, false, false, false, false, false, false, false, false, false, false, false };
		unsigned int faceColors[6] = { COLORS[0], COLORS[0], COLORS[0], COLORS[0], COLORS[0], COLORS[0] };
		FillSpongeBuffers(0, levelMax, vertices, indices, Vector3(), aoEnabled, aoEdges, faceColors);

		SPtr<VertexBuffer> pVertexBuffer(new VertexBuffer());
		pVertexBuffer->Create(std::move(vertices));

		SPtr<IndexBuffer> pIndexBuffer(new IndexBuffer(std::move(indices)));

		SPtr<Geometry> geo(new Geometry());
		geo->SetNumVertexBuffers(1);
		geo->SetVertexBuffer(0, pVertexBuffer);
		geo->SetIndexBuffer(pIndexBuffer);
		geo->SetDrawRange(PrimitiveTopology::TRIANGLE_LIST, 0, (unsigned int)indices.size());
		return geo;
	}

	// Append vertices and indices of a cube to the index and vertex buffers.
	// The cube has gradient ambient-occlusion defined per edge.
	void AppendCubeToBuffers(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const Matrix3x4& xform, float aoRatio, const bool aoEdges[12],
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
		const Vector3 A[6] = { Vector3(-R, -R, -R), Vector3(+R, -R, -R), Vector3(+R, -R, +R), Vector3(-R, -R, +R), Vector3(-R, +R, -R), Vector3(-R, -R, -R) };
		const Vector3 B[6] = { Vector3(+R, -R, -R), Vector3(+R, -R, +R), Vector3(-R, -R, +R), Vector3(-R, -R, -R), Vector3(+R, +R, -R), Vector3(+R, -R, -R) };
		const Vector3 C[6] = { Vector3(-R, +R, -R), Vector3(+R, +R, -R), Vector3(+R, +R, +R), Vector3(-R, +R, +R), Vector3(-R, +R, +R), Vector3(-R, -R, +R) };
		const Vector3 D[6] = { Vector3(+R, +R, -R), Vector3(+R, +R, +R), Vector3(-R, +R, +R), Vector3(-R, +R, -R), Vector3(+R, +R, +R), Vector3(+R, -R, +R) };
		// the 6 face normals
		const Vector3 N[6] = { Vector3(0,  0, -1), Vector3(+1,  0,  0), Vector3(0,  0, +1), Vector3(-1,  0,  0), Vector3(0, +1,  0), Vector3(0, -1,  0) };
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
					vertex.Position = xform * vertex.Position;

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
		const Vector3& center, bool aoEnabled, const bool aoEdges[12], const unsigned int faceColors[6])
	{
		float scale = pow(1.0f / 3.0f, level);

		if (level == levelMax)
		{
			float aoRatio = pow(3.0f, level) * 0.02f;
			if (aoRatio > 0.4999f)
				aoRatio = 0.4999f;
			Matrix3x4 xform(center, Quaternion::IDENTITY, scale);
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
							Vector3 t(center.x_ + s * i, center.y_ + s * j, center.z_ + s * k);

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


}
