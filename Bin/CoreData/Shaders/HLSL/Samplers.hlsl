// Make sampling macros also available for VS on D3D11
#define Sample2D(tex, uv) tex.Sample(tex##_sampler, uv)
#define Sample2DProj(tex, uv) tex.Sample(tex##_sampler, uv.xy / uv.w)
#define Sample2DLod0(tex, uv) tex.SampleLevel(tex##_sampler, uv, 0.0)
#define SampleCube(tex, uv) tex.Sample(tex##_sampler, uv)
#define SampleCubeLOD(tex, uv) tex.SampleLevel(tex##_sampler, uv.xyz, uv.w)
#define SampleShadow(tex, uv) tex.SampleCmpLevelZero(tex##_sampler, uv.xy, uv.z)

// D3D11 textures and samplers

Texture2D DiffMap;// : register(t0);
TextureCube DiffCubeMap;// : register(t0);
Texture2D AlbedoBuffer;// : register(t0);
Texture2D NormalMap;// : register(t1);
Texture2D NormalBuffer;// : register(t1);
Texture2D SpecMap;// : register(t2);
Texture2D RoughMetalFresnel;// : register(t2); //R: Roughness, G: Metal
Texture2D EmissiveMap;// : register(t3);
Texture2D EnvMap;// : register(t4);
Texture3D VolumeMap;// : register(t5);
TextureCube EnvCubeMap;// : register(t4);
Texture2D LightRampMap;// : register(t8);
Texture2D LightSpotMap;// : register(t9);
TextureCube LightCubeMap;// : register(t9);
Texture2D ShadowMap;// : register(t10);
TextureCube FaceSelectCubeMap;// : register(t11);
TextureCube IndirectionCubeMap;// : register(t12);
Texture2D DepthBuffer;// : register(t13);
Texture2D LightBuffer;// : register(t14);
TextureCube ZoneCubeMap;// : register(t15);
Texture3D ZoneVolumeMap;// : register(t15);

SamplerState DiffMap_sampler;// : register(s0);
SamplerState DiffCubeMap_sampler;// : register(s0);
SamplerState AlbedoBuffer_sampler;// : register(s0);
SamplerState NormalMap_sampler;// : register(s1);
SamplerState NormalBuffer_sampler;// : register(s1);
SamplerState SpecMap_sampler;// : register(s2);
SamplerState RoughMetalFresnel_sampler;// : register(s2); //R: Roughness, G: Metal 
SamplerState EmissiveMap_sampler;// : register(s3);
SamplerState EnvMap_sampler;// : register(s4);
SamplerState VolumeMap_sampler;// : register(s5);
SamplerState EnvCubeMap_sampler;// : register(s4);
SamplerState LightRampMap_sampler;// : register(s8);
SamplerState LightSpotMap_sampler;// : register(s9);
SamplerState LightCubeMap_sampler;// : register(s9);
#ifdef VSM_SHADOW
    SamplerState ShadowMap_sampler;// : register(s10);
#else
    SamplerComparisonState ShadowMap_sampler;// : register(s10);
#endif
SamplerState FaceSelectCubeMap_sampler;// : register(s11);
SamplerState IndirectionCubeMap_sampler;// : register(s12);
SamplerState DepthBuffer_sampler;// : register(s13);
SamplerState LightBuffer_sampler;// : register(s14);
SamplerState ZoneCubeMap_sampler;// : register(s15);
SamplerState ZoneVolumeMap_sampler;// : register(s15);

float3 DecodeNormal(float4 normalInput)
{
#ifdef PACKEDNORMAL
    float3 normal;
    normal.xy = normalInput.ag * 2.0 - 1.0;
    normal.z = sqrt(max(1.0 - dot(normal.xy, normal.xy), 0.0));
    return normal;
#else
    return normalInput.rgb * 2.0 - 1.0;
#endif
}

#ifdef COMPILEPS

float ReconstructDepth(float hwDepth)
{
    return dot(float2(hwDepth, cDepthReconstruct.y / (hwDepth - cDepthReconstruct.x)), cDepthReconstruct.zw);
}
#endif

