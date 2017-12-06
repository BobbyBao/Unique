
/* * * * * * * * * * * * * Author's note * * * * * * * * * * * *\
*   _       _   _       _   _       _   _       _     _ _ _ _   *
*  |_|     |_| |_|     |_| |_|_   _|_| |_|     |_|  _|_|_|_|_|  *
*  |_|_ _ _|_| |_|     |_| |_|_|_|_|_| |_|     |_| |_|_ _ _     *
*  |_|_|_|_|_| |_|     |_| |_| |_| |_| |_|     |_|   |_|_|_|_   *
*  |_|     |_| |_|_ _ _|_| |_|     |_| |_|_ _ _|_|  _ _ _ _|_|  *
*  |_|     |_|   |_|_|_|   |_|     |_|   |_|_|_|   |_|_|_|_|    *
*                                                               *
*                     http://www.humus.name                     *
*                                                                *
* This file is a part of the work done by Humus. You are free to   *
* use the code in any way you like, modified, unmodified or copied   *
* into your own work. However, I expect you to respect these points:  *
*  - If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  - For use in anything commercial, please request my approval.     *
*  - Share your work and ideas too as much as you can.             *
*                                                                *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "App.h"

const uint CLUSTER_X = 32;
const uint CLUSTER_Y = 8;
const uint CLUSTER_Z = 32;

BaseApp *app = new App();

bool App::onKey(const uint key, const bool pressed)
{
	if (D3D11App::onKey(key, pressed))
		return true;

	if (pressed)
	{
		if (key >= KEY_F5 && key <= KEY_F8)
		{
		//	m_RenderMode->selectItem(key - KEY_F5);
			return true;
		}
	}

	return false;
}

void App::moveCamera(const float3 &dir)
{
	float3 newPos = camPos + dir * (speed * frameTime);

	float3 point;
	const BTri *tri;
	if (m_BSP.intersects(camPos, newPos, &point, &tri))
	{
		newPos = point + tri->plane.xyz();
	}
	m_BSP.pushSphere(newPos, 35);

	camPos = newPos;
}

void App::resetCamera()
{
	camPos = vec3(-985, 105, -860);
	wx = 0.16f;
	wy = -0.91f;
}

void App::onSize(const int w, const int h)
{
	D3D11App::onSize(w, h);

	if (renderer)
	{
		// Make sure render targets are the size of the window
		renderer->resizeRenderTarget(m_BaseRT,      w, h, 1, 1, 1);
		renderer->resizeRenderTarget(m_NormalRT,    w, h, 1, 1, 1);
		renderer->resizeRenderTarget(m_DepthRT,     w, h, 1, 1, 1);
		renderer->resizeRenderTarget(m_StencilMask, w, h, 1, 1, 1);
	}
}

bool App::init()
{
	// No framework created depth buffer
	depthBits = 0;

	if (!m_Map.loadObj("Assets/Models/Corridor2/Map.obj")) return false;
	m_Map.scale(0, float3(1, 1, -1));

	uint nIndices = m_Map.getIndexCount();
	float3 *src = (float3 *) m_Map.getStream(0).vertices;
	uint *inds = m_Map.getStream(0).indices;

	uint nVertices = m_Map.getStream(0).nVertices;
	float3 aabb_min = src[0];
	float3 aabb_max = src[0];
	for (uint i = 1; i < nVertices; i++)
	{
		aabb_min.x = min(aabb_min.x, src[i].x);
		aabb_min.y = min(aabb_min.y, src[i].y);
		aabb_min.z = min(aabb_min.z, src[i].z);
		aabb_max.x = max(aabb_max.x, src[i].x);
		aabb_max.y = max(aabb_max.y, src[i].y);
		aabb_max.z = max(aabb_max.z, src[i].z);
	}
	m_AABBMin = aabb_min;
	m_AABBMax = aabb_max + 1.0f; // Tiny nudge factor for walls and ceiling exactly parallel with AABBMax that'd otherwise fall outside of AABB

	for (uint i = 0; i < nIndices; i += 3)
	{
		float3 v0 = src[inds[i]];
		float3 v1 = src[inds[i + 1]];
		float3 v2 = src[inds[i + 2]];

		m_BSP.addTriangle(v0, v1, v2);
	}
	m_BSP.build();

	m_Map.computeTangentSpace(true);

	m_Sphere.createSphere(3);

	// Initialize all lights
	m_Lights[ 0] = { float3( 576, 96,    0), 640.0f };
	m_Lights[ 1] = { float3( 0,   96,  576), 640.0f };
	m_Lights[ 2] = { float3(-576, 96,    0), 640.0f };
	m_Lights[ 3] = { float3( 0,   96, -576), 640.0f };
	m_Lights[ 4] = { float3(1792, 96,  320), 550.0f };
	m_Lights[ 5] = { float3(1792, 96, -320), 550.0f };
	m_Lights[ 6] = { float3(-192, 96, 1792), 550.0f };
	m_Lights[ 7] = { float3(-832, 96, 1792), 550.0f };
	m_Lights[ 8] = { float3(1280, 32,  192), 450.0f };
	m_Lights[ 9] = { float3(1280, 32, -192), 450.0f };
	m_Lights[10] = { float3(-320, 32, 1280), 450.0f };
	m_Lights[11] = { float3(-704, 32, 1280), 450.0f };
	m_Lights[12] = { float3( 960, 32,  640), 450.0f };
	m_Lights[13] = { float3( 960, 32, -640), 450.0f };
	m_Lights[14] = { float3( 640, 32, -960), 450.0f };
	m_Lights[15] = { float3(-640, 32, -960), 450.0f };
	m_Lights[16] = { float3(-960, 32,  640), 450.0f };
	m_Lights[17] = { float3(-960, 32, -640), 450.0f };
	m_Lights[18] = { float3( 640, 32,  960), 450.0f };
	// Dynamic lights
	m_Lights[19] = { float3(   0,  0,    0), 350.0f };
	m_Lights[20] = { float3(   0,  0,    0), 650.0f };
	m_Lights[21] = { float3(   0,  0,    0), 500.0f };

	return true;
}

enum RenderMode
{
	CLUSTERED_SHADING,
	VISUALIZE_CLUSTERS,
	DEFERRED_SHADING,
	VISUALIZE_STENCIL,
};

void App::exit()
{
}

bool App::initAPI()
{
	// Override the user's MSAA settings
	return D3D11App::initAPI(D3D10_1, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, DXGI_FORMAT_UNKNOWN, 1, NO_SETTING_CHANGE | SAMPLE_BACKBUFFER);
}

void App::exitAPI()
{
	D3D11App::exitAPI();
}

bool App::load()
{
	int sampleCount = max(antiAliasSamples, 1);

	char def[256];
	sprintf(def, "#define SAMPLE_COUNT %d\n", sampleCount);

	// Shaders
	if ((m_Clustered    = renderer->addShader("Clustered.shd", DEFINE_STR(LIGHT_COUNT))) == SHADER_NONE) return false;
	if ((m_ShowClusters = renderer->addShader("Clustered.shd", DEFINE_STR(LIGHT_COUNT) "#define SHOW_CLUSTERS\n")) == SHADER_NONE) return false;
	if ((m_PreZ         = renderer->addShader("PreZ.shd")) == SHADER_NONE) return false;
	if ((m_FillBuffers  = renderer->addShader("FillBuffers.shd")) == SHADER_NONE) return false;
	if ((m_CreateMask   = renderer->addShader("CreateMask.shd")) == SHADER_NONE) return false;
	if ((m_Ambient[0]   = renderer->addShader("Ambient.shd", def)) == SHADER_NONE) return false;
	if ((m_Lighting[0]  = renderer->addShader("Lighting.shd", def)) == SHADER_NONE) return false;
	strcat(def, "#define SINGLE_SAMPLE\n");
	if ((m_Ambient[1]   = renderer->addShader("Ambient.shd", def)) == SHADER_NONE) return false;
	if ((m_Lighting[1]  = renderer->addShader("Lighting.shd", def)) == SHADER_NONE) return false;
	if ((m_LightBlob[0] = renderer->addShader("LightBlob.shd")) == SHADER_NONE) return false;
	if ((m_LightBlob[1] = renderer->addShader("LightBlob.shd", "#define DEFERRED\n")) == SHADER_NONE) return false;

	// Samplerstates
	if ((m_TrilinearAniso = renderer->addSamplerState(TRILINEAR_ANISO, WRAP, WRAP, WRAP)) == SS_NONE) return false;
	if ((m_PointClamp = renderer->addSamplerState(NEAREST, CLAMP, CLAMP, CLAMP)) == SS_NONE) return false;

	// Main render targets
	if ((m_BaseRT      = renderer->addRenderTarget(width, height, 1, 1, 1, FORMAT_RGBA8,   sampleCount, SS_NONE, SRGB)) == TEXTURE_NONE) return false;
	if ((m_NormalRT    = renderer->addRenderTarget(width, height, 1, 1, 1, FORMAT_RGBA8S,  sampleCount, SS_NONE)) == TEXTURE_NONE) return false;
	if ((m_DepthRT     = renderer->addRenderDepth (width, height, 1,       FORMAT_D16,     sampleCount, SS_NONE, SAMPLE_DEPTH)) == TEXTURE_NONE) return false;
	if ((m_StencilMask = renderer->addRenderDepth (width, height, 1,       FORMAT_D24S8,   1,           SS_NONE)) == TEXTURE_NONE) return false;

	// Create a dynamic texture for the clusters. We're using a 32bit integer format where each set bit enables the light of that index.
	// This supports up to 32 lights, which is enough for this demo, and probably for some games. It's possible to expand if more lights are needed,
	// for instance RGBA32_UINT for up to 128 lights in a single fetch, which is enough for many AAA titles. At some point, a list of indices becomes
	// more compact in practice, so if thousands of lights are needed, that's probably the way to go. Using a fixed bitmask has the advantage of fixed
	// size storage, simple addressing, and one indirection less in the inner loop.
	D3D11_TEXTURE3D_DESC desc;
	desc.Width  = CLUSTER_X;
	desc.Height = CLUSTER_Y;
	desc.Depth  = CLUSTER_Z;
    desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32_UINT;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	ID3D11Texture3D* tex = NULL;
	if (FAILED(device->CreateTexture3D(&desc, NULL, &tex))) return false;
	m_Clusters = ((Direct3D11Renderer *) renderer)->addTexture(tex);

	// Textures
	if ((m_Base[0] = renderer->addTexture  ("Assets/Textures/Tx_wood_brown_shelf_small.dds",                    true, m_TrilinearAniso, SRGB)) == TEXTURE_NONE) return false;
	if ((m_Bump[0] = renderer->addNormalMap("Assets/Textures/Tx_wood_brown_shelf_smallBump.dds", FORMAT_RGBA8S, true, m_TrilinearAniso)) == TEXTURE_NONE) return false;

	if ((m_Base[1] = renderer->addTexture  ("Assets/Textures/wallpaper20.dds",                    true, m_TrilinearAniso, SRGB)) == TEXTURE_NONE) return false;
	if ((m_Bump[1] = renderer->addNormalMap("Assets/Textures/wallpaper20Bump.dds", FORMAT_RGBA8S, true, m_TrilinearAniso)) == TEXTURE_NONE) return false;

	if ((m_Base[2] = renderer->addTexture  ("Assets/Textures/floor_wood_4.dds",                    true, m_TrilinearAniso, SRGB)) == TEXTURE_NONE) return false;
	if ((m_Bump[2] = renderer->addNormalMap("Assets/Textures/floor_wood_4Bump.dds", FORMAT_RGBA8S, true, m_TrilinearAniso)) == TEXTURE_NONE) return false;

	if ((m_Base[3] = renderer->addTexture  ("Assets/Textures/floor_wood_3.dds",                    true, m_TrilinearAniso, SRGB)) == TEXTURE_NONE) return false;
	if ((m_Bump[3] = renderer->addNormalMap("Assets/Textures/floor_wood_3Bump.dds", FORMAT_RGBA8S, true, m_TrilinearAniso)) == TEXTURE_NONE) return false;

	if ((m_Base[4] = renderer->addTexture  ("Assets/Textures/light2.dds",                    true, m_TrilinearAniso, SRGB)) == TEXTURE_NONE) return false;
	if ((m_Bump[4] = renderer->addNormalMap("Assets/Textures/light2Bump.dds", FORMAT_RGBA8S, true, m_TrilinearAniso)) == TEXTURE_NONE) return false;

	// Blendstates
	if ((m_BlendAdd = renderer->addBlendState(ONE, ONE)) == BS_NONE) return false;

	// Depth states - use reversed depth (1 to 0) to improve precision
	if ((m_DepthTest = renderer->addDepthState(true, true, GEQUAL)) == DS_NONE) return false;
	if ((m_StencilSet = renderer->addDepthState(false, false, GEQUAL, true, 0xFF, ALWAYS, REPLACE, REPLACE, REPLACE)) == DS_NONE) return false;
	if ((m_StencilTest = renderer->addDepthState(false, false, GEQUAL, true, 0xFF, EQUAL, KEEP, KEEP, KEEP)) == DS_NONE) return false;

	// Upload map to vertex/index buffer
	if (!m_Map.makeDrawable(renderer, true, m_FillBuffers)) return false;
	if (!m_Sphere.makeDrawable(renderer, true, m_Lighting[0])) return false;

	return true;
}

void App::unload()
{
}

void App::animateLights()
{
	static const float3 light_path[] =
	{
		float3(-170.6f,   53.9f, 1849.3f),
		float3(-546.0f,  -66.9f, 1379.1f),
		float3(-439.8f,  -77.5f,  945.9f),
		float3( 177.9f,   20.3f,  766.3f),
		float3( 674.2f,  168.5f,  522.0f),
		float3( 911.7f,  139.8f,   69.1f),
		float3(1489.7f,  108.8f,   28.5f),
		float3(1789.9f, -129.8f,  304.0f),
		float3(1890.4f, -123.3f,  -83.5f),
		float3(1572.9f,   92.8f, -131.2f),
		float3(1101.8f,   33.3f,  -73.5f),
		float3( 593.9f,   -9.3f, -554.3f),
		float3( 007.0f,  -53.5f, -693.1f),
		float3(-766.0f,  -14.6f, -640.8f),
		float3(-595.7f,  -53.4f,  750.0f),
		float3(-684.6f, -138.9f, 1663.3f),
		float3(-890.4f,   82.8f, 1904.6f),
		float3(-602.9f,  145.0f, 1932.6f),
		float3(-282.4f,   88.6f, 1908.3f),
	};


	for (uint i = LIGHT_COUNT - 3; i < LIGHT_COUNT; i++)
	{
		float t = time + 6 * i;

		int node = (int) t;
		float f = t - node;

		float3 p0 = light_path[(node + 0) % elementsOf(light_path)];
		float3 p1 = light_path[(node + 1) % elementsOf(light_path)];
		float3 p2 = light_path[(node + 2) % elementsOf(light_path)];
		float3 p3 = light_path[(node + 3) % elementsOf(light_path)];

		m_Lights[i].Position = cerp(p0, p1, p2, p3, f);
	}
}

void App::drawLights(const float4x4& view_proj, bool deferred_shader)
{
	renderer->reset();
	renderer->setRasterizerState(cullFront);
	renderer->setShader(m_LightBlob[deferred_shader? 1 : 0]);
	renderer->setShaderConstant4x4f("ViewProj", view_proj);
	renderer->setDepthState(m_DepthTest);
	renderer->apply();

	for (int i = LIGHT_COUNT - 3; i < LIGHT_COUNT; i++)
	{
		renderer->setShaderConstant3f("LightPos", m_Lights[i].Position);
		renderer->applyConstants();

		m_Sphere.draw(renderer);
	}
}

void App::clusteredLightAssignment()
{
	uint32 lights[CLUSTER_Z][CLUSTER_Y][CLUSTER_X] = {};

	float3 scale = float3(float(CLUSTER_X), float(CLUSTER_Y), float(CLUSTER_Z)) / (m_AABBMax - m_AABBMin);
	float3 inv_scale = 1.0f / scale;

	for (uint i = 0; i < LIGHT_COUNT; i++)
	{
		const Light& light = m_Lights[i];

		const float3 p = (light.Position - m_AABBMin);
		const float3 p_min = (p - light.Radius) * scale;
		const float3 p_max = (p + light.Radius) * scale;

		// Cluster for the center of the light
		const int px = (int) floorf(p.x * scale.x);
		const int py = (int) floorf(p.y * scale.y);
		const int pz = (int) floorf(p.z * scale.z);

		// Cluster bounds for the light
		const int x0 = max((int) floorf(p_min.x), 0);
		const int x1 = min((int) ceilf(p_max.x), CLUSTER_X);
		const int y0 = max((int) floorf(p_min.y), 0);
		const int y1 = min((int) ceilf(p_max.y), CLUSTER_Y);
		const int z0 = max((int) floorf(p_min.z), 0);
		const int z1 = min((int) ceilf(p_max.z), CLUSTER_Z);

		const float radius_sqr = light.Radius * light.Radius;
		const uint32 mask = (1 << i);

		// Do AABB<->Sphere tests to figure out which clusters are actually intersected by the light
		for (int z = z0; z < z1; z++)
		{
			float dz = (pz == z)? 0.0f : m_AABBMin.z + (pz < z? z : z + 1) * inv_scale.z - light.Position.z;
			dz *= dz;

			for (int y = y0; y < y1; y++)
			{
				float dy = (py == y)? 0.0f : m_AABBMin.y + (py < y? y : y + 1) * inv_scale.y - light.Position.y;
				dy *= dy;
				dy += dz;

				for (int x = x0; x < x1; x++)
				{
					float dx = (px == x)? 0.0f : m_AABBMin.x + (px < x? x : x + 1) * inv_scale.x - light.Position.x;
					dx *= dx;
					dx += dy;

					if (dx < radius_sqr)
					{
						lights[z][y][x] |= mask;
					}
				}
			}
		}
	}

	// Upload the cluster data to a volume texture
	ID3D11Resource* tex = ((Direct3D11Renderer *) renderer)->getResource(m_Clusters);

	D3D11_MAPPED_SUBRESOURCE map;
	context->Map(tex, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	for (uint z = 0; z < CLUSTER_Z; z++)
	{
		for (uint y = 0; y < CLUSTER_Y; y++)
		{
			memcpy(((uint8 *) map.pData) + z * map.DepthPitch + y * map.RowPitch, lights[z][y], CLUSTER_X * sizeof(uint32));
		}
	}
	context->Unmap(tex, 0);
}

void App::drawClustered(const float4x4& view_proj, bool show_clusters)
{
	clusteredLightAssignment();

	renderer->changeRenderTargets(NULL, 0, m_DepthRT);

		renderer->clear(false, true, false, NULL, 0.0f);

		/*
			Pre-Z pass for a minor performance increase.
		*/
		renderer->reset();
		renderer->setRasterizerState(cullFront);
		renderer->setShader(m_PreZ);
		renderer->setShaderConstant4x4f("ViewProj", view_proj);
		renderer->setDepthState(m_DepthTest);
		renderer->apply();

		m_Map.draw(renderer);

	renderer->changeRenderTarget((antiAliasSamples > 1)? m_BaseRT : FB_COLOR, m_DepthRT);

		float3 scale = float3(float(CLUSTER_X), float(CLUSTER_Y), float(CLUSTER_Z)) / (m_AABBMax - m_AABBMin);

		/*
			Clustered shading pass. Done as a forward pass.
		*/
		renderer->reset();
		renderer->setRasterizerState(cullBack);
		renderer->setShader(show_clusters? m_ShowClusters : m_Clustered);
		renderer->setShaderConstant4x4f("ViewProj", view_proj);
		renderer->setShaderConstant3f("CamPos", camPos);
		renderer->setShaderConstant3f("Scale", scale);
		renderer->setShaderConstant3f("Bias", -scale * m_AABBMin);
		renderer->setShaderConstantArray4f("Lights", (const float4 *) m_Lights, LIGHT_COUNT);
		renderer->setSamplerState("Filter", m_TrilinearAniso);
		renderer->setDepthState(m_DepthTest);
		renderer->apply();

		for (uint i = 0; i < m_Map.getBatchCount(); i++)
		{
			renderer->setTexture("Base", m_Base[i]);
			renderer->setTexture("Bump", m_Bump[i]);
			renderer->setTexture("Clusters", m_Clusters);
			renderer->applyTextures();

			m_Map.drawBatch(renderer, i);
		}

		drawLights(view_proj, false);

	renderer->changeToMainFramebuffer();

	if (antiAliasSamples > 1)
		context->ResolveSubresource(backBuffer, 0, ((Direct3D11Renderer*)renderer)->getResource(m_BaseRT), 0, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

}

void App::drawClassicDeferred(const float4x4& view_proj, const float4x4& view, const float4x4& projection, const float near_plane, bool visualize_stencil)
{
	// Pre-scale-bias the matrix so I can use the screen position directly
	float4x4 view_proj_inv = (!view_proj) * (translate(-1.0f, 1.0f, 0.0f) * scale(2.0f / width, -2.0f / height, 1.0f));

	TextureID bufferRTs[] = { m_BaseRT, m_NormalRT };
	renderer->changeRenderTargets(bufferRTs, elementsOf(bufferRTs), m_DepthRT);
		renderer->clear(false, true, false, NULL, 0.0f);

		/*
			Main scene pass.
			This is where the buffers are filled for the later deferred passes.
		*/
		renderer->reset();
		renderer->setRasterizerState(cullBack);
		renderer->setShader(m_FillBuffers);
		renderer->setShaderConstant4x4f("ViewProj", view_proj);
		renderer->setShaderConstant3f("CamPos", camPos);
		renderer->setSamplerState("Filter", m_TrilinearAniso);
		renderer->setDepthState(m_DepthTest);
		renderer->apply();

		for (uint i = 0; i < m_Map.getBatchCount(); i++)
		{
			renderer->setTexture("Base", m_Base[i]);
			renderer->setTexture("Bump", m_Bump[i]);
			renderer->applyTextures();

			m_Map.drawBatch(renderer, i);
		}

		drawLights(view_proj, true);

	renderer->changeRenderTargets(NULL, 0, m_StencilMask);
		renderer->clear(false, true, true, NULL, 0.0f, 0);

		/*
			Create the stencil mask
		*/
		renderer->reset();
		renderer->setRasterizerState(cullNone);
		renderer->setShader(m_CreateMask);
		renderer->setTexture("BackBuffer", backBufferTexture);
		renderer->setSamplerState("Filter", m_PointClamp);
		renderer->setDepthState(m_StencilSet, 0x1);
		renderer->apply();

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->Draw(3, 0);

	renderer->changeRenderTarget(FB_COLOR, m_StencilMask);




	/*
		Deferred lighting pass.
		Draw twice, using stencil to separate pixels for single or multiple sample evaluation.
	*/
	float2 zw = projection.rows[2].zw();

	int passCount = (!visualize_stencil && antiAliasSamples > 1)? 2 : 1;

	for (int p = 0; p < passCount; p++)
	{
		/*
			Deferred ambient pass
		*/
		renderer->reset();
		if (!visualize_stencil && antiAliasSamples > 1)
		{
			renderer->setDepthState(m_StencilTest, (p == 0)? 0x1 : 0x0);
			renderer->setShader(m_Ambient[p]);
		}
		else
		{
			renderer->setDepthState(noDepthTest);
			renderer->setShader(m_Ambient[1]);
		}
		renderer->setRasterizerState(cullNone);
		renderer->setShaderConstant1f("Factor", visualize_stencil? 1.0f : 0.0f);
		renderer->setTexture("Base", m_BaseRT);
		if (antiAliasSamples <= 1)
			renderer->setSamplerState("Filter", m_PointClamp);
		renderer->apply();


		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->Draw(3, 0);



		/*
			Deferred lighting pass
		*/
		renderer->reset();
		if (!visualize_stencil && antiAliasSamples > 1)
		{
			renderer->setDepthState(m_StencilTest, (p == 0)? 0x1 : 0x0);
			renderer->setShader(m_Lighting[p]);
		}
		else
		{
			renderer->setDepthState(noDepthTest);
			renderer->setShader(m_Lighting[1]);
		}
		renderer->setRasterizerState(cullFront);
		renderer->setBlendState(m_BlendAdd);
		renderer->setShaderConstant4x4f("ViewProj", view_proj);
		renderer->setShaderConstant4x4f("ViewProjInv", view_proj_inv);
		renderer->setShaderConstant3f("CamPos", camPos);
		renderer->setTexture("Base", m_BaseRT);
		renderer->setTexture("Normal", m_NormalRT);
		renderer->setTexture("Depth", m_DepthRT);
		renderer->apply();

		for (uint i = 0; i < LIGHT_COUNT; i++)
		{
			const float3& lightPos = m_Lights[i].Position;
			const float radius = m_Lights[i].Radius;
			const float inv_radius_sqr = 1.0f / (radius * radius);

			// Compute z-bounds
			float4 lPos = view * float4(lightPos, 1.0f);
			float z1 = lPos.z + radius;

			if (z1 > near_plane)
			{
				float z0 = max(lPos.z - radius, near_plane);

				float2 zBounds;
				zBounds.y = saturate(zw.x + zw.y / z0);
				zBounds.x = saturate(zw.x + zw.y / z1);

				renderer->setShaderConstant3f("LightPos", lightPos);
				renderer->setShaderConstant1f("Radius", radius);
				renderer->setShaderConstant1f("InvRadiusSqr", inv_radius_sqr);
				renderer->setShaderConstant2f("ZBounds", zBounds);
				renderer->applyConstants();

				m_Sphere.draw(renderer);
			}
		}
	}
}

void App::drawFrame()
{
	const float near_plane = 20.0f;
	const float far_plane = 4000.0f;

	// Reversed depth
	float4x4 projection = toD3DProjection(perspectiveMatrixY(1.2f, width, height, far_plane, near_plane));
	float4x4 view = rotateXY(-wx, -wy);
	view.translate(-camPos);
	float4x4 view_proj = projection * view;

	animateLights();

	RenderMode mode = CLUSTERED_SHADING;// (RenderMode)m_RenderMode->getSelectedItem();

	if (mode <= VISUALIZE_CLUSTERS)
	{
		drawClustered(view_proj, (mode == VISUALIZE_CLUSTERS));
	}
	else
	{
		drawClassicDeferred(view_proj, view, projection, near_plane, (mode == VISUALIZE_STENCIL));
	}
}
