#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"

cbuffer Constants
{
    float4x4 g_WorldViewProj;
	float4x3 g_WorldNorm;
    float3 g_LightDir;
    float g_LightCoeff;
};

struct PSInput 
{ 
    float4 Pos : SV_POSITION; 
};

PSInput VS(float4 iPos : ATTRIB0, float3 norm : ATTRIB1) 
{   
 PSInput ps; 
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos =iPos.xyz;// GetWorldPos(modelMatrix);
    ps.Pos = GetClipPos(worldPos);

    //ps.Pos = mul( iPos, g_WorldViewProj);
    return ps;
}

float4 PS(PSInput input) : SV_TARGET
{
    return float4(1, 0, 0, 1); 
}
