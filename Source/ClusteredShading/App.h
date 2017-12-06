
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

#include "../UniqueRenderer/Direct3D11/D3D11App.h"
#include "../UniqueRenderer/Util/Model.h"
#include "../UniqueRenderer/Util/BSP.h"

struct Light
{
	float3 Position;
	float  Radius;
};

#define LIGHT_COUNT 22

class App : public D3D11App
{
public:
	char *getTitle() const { return "Clustered Shading"; }

	bool onKey(const uint key, const bool pressed);

	void moveCamera(const float3 &dir);
	void resetCamera();

	void onSize(const int w, const int h);

	bool init();
	void exit();

	bool initAPI();
	void exitAPI();

	bool load();
	void unload();

	void clusteredLightAssignment();
	void animateLights();
	void drawLights(const float4x4& view_proj, bool deferred_shader);
	void drawClustered(const float4x4& view_proj, bool show_clusters);
	void drawClassicDeferred(const float4x4& view_proj, const float4x4& view, const float4x4& projection, const float near_plane, bool visualize_stencil);
	void drawFrame();

protected:
	ShaderID m_Clustered, m_ShowClusters, m_PreZ, m_FillBuffers, m_Ambient[2], m_Lighting[2], m_CreateMask, m_LightBlob[2];
	TextureID m_Base[5], m_Bump[5];
	TextureID m_Clusters, m_BaseRT, m_NormalRT, m_DepthRT, m_StencilMask;

	SamplerStateID m_TrilinearAniso, m_PointClamp;
	BlendStateID m_BlendAdd;
	DepthStateID m_DepthTest, m_StencilSet, m_StencilTest;

	Light m_Lights[LIGHT_COUNT];
	float3 m_AABBMin, m_AABBMax;

	Model m_Map;
	Model m_Sphere;
	BSP m_BSP;

	DropDownList* m_RenderMode;
};
