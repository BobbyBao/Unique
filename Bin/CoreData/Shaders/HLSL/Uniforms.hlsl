
#ifdef COMPILEVS

// Vertex shader uniforms
cbuffer FrameVS
{
    float DeltaTime;
    float ElapsedTime;
}

cbuffer CameraVS
{  
	float4x3 View;
    float4x3 ViewInv;
    float4x4 ViewProj;
    float3 CameraPos;
    float NearClip;
    float FarClip;
    float3 FrustumSize;
    float4 DepthMode;
    float4 GBufferOffsets;
    float4 ClipPlane;
}

cbuffer ZoneVS
{
    float3 AmbientStartColor;
    float3 AmbientEndColor;
    float4x3 Zone;
}

cbuffer LightVS
{
    float4 LightPos;
    float3 LightDir;
    float4 NormalOffsetScale;
#ifdef NUMVERTEXLIGHTS
    float4 VertexLights[4 * 3];
#else
    float4x4 LightMatrices[4];
#endif
}

#ifndef CUSTOM_MATERIAL_CBUFFER
cbuffer MaterialVS
{
    float4 UOffset;
    float4 VOffset;
}
#endif

cbuffer ObjectVS
{
    float4x3 Model;
}

#ifndef MAXBONES
#define MAXBONES 64
#endif

cbuffer SkinnedVS
{
    uniform float4x3 SkinMatrices[MAXBONES];
}

cbuffer BillboardVS
{
	float4x3 World;
	float3x3 BillboardRot;
}

#endif

#ifdef COMPILEPS

// Pixel shader uniforms
cbuffer FramePS
{
    float DeltaTimePS;
    float ElapsedTimePS;
}

cbuffer CameraPS
{
    float3 CameraPosPS;
    float4 DepthReconstruct;
    float2 GBufferInvSize;
    float NearClipPS;
    float FarClipPS;
}

cbuffer ZonePS
{
    float4 AmbientColor;
    float4 FogParams;
    float3 FogColor;
    float3 ZoneMin;
    float3 ZoneMax;
}

cbuffer LightPS
{
    float4 LightColor;
    float4 LightPosPS;
    float3 LightDirPS;
    float4 NormalOffsetScalePS;
    float4 ShadowCubeAdjust;
    float4 ShadowDepthFade;
    float2 ShadowIntensity;
    float2 ShadowMapInvSize;
    float4 ShadowSplits;
    float2 VSMShadowParams;
    float4x4 LightMatricesPS[4];
    #ifdef PBR
        float LightRad;
        float LightLength;
    #endif
}

#ifndef CUSTOM_MATERIAL_CBUFFER
cbuffer MaterialPS
{
    float4 MatDiffColor;
    float3 MatEmissiveColor;
    float3 MatEnvMapColor;
    float4 MatSpecColor;
	float Roughness;
	float Metallic;
}
#endif

#endif

