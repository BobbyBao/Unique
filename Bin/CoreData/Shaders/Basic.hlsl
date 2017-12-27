#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"

struct PSInput 
{ 
    float4 Pos : SV_POSITION; 
};

PSInput VS(float4 iPos : ATTRIB0, float3 norm : ATTRIB1) 
{   
	PSInput ps; 
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    ps.Pos = GetClipPos(worldPos);

    return ps;
}

float4 PS(PSInput input) : SV_TARGET
{
    return float4(1, 0, 0, 1); 
}
