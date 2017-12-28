#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"


void VS(
		float4 iPos : ATTRIB0, 
		float4 iColor : ATTRIB1,
        out float4 oColor : COLOR0,
		out float4 oPos : OUTPOSITION
    ) 
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
	oColor = iColor;
}

float4 PS(float4 iColor : COLOR0) : SV_TARGET
{
    return iColor; 
}
