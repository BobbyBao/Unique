#include "Uniforms.hlsl"
#include "Transform.hlsl"


void VS(
		float3 iPos : ATTRIB0, 
		out float4 oPos : SV_POSITION
    ) 
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
}

float4 PS() : SV_TARGET
{
    return float4(1.0, 0.0, 1.0, 1); 
}
