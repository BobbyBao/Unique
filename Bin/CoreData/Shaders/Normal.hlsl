#include "Uniforms.hlsl"
//#include "Samplers.hlsl"
#include "Transform.hlsl"

Texture2D DiffMap;
SamplerState DiffMap_sampler;

void VS(
		float4 iPos : ATTRIB0, 
        float3 iNormal : ATTRIB1,
        float4 iColor : ATTRIB2,
        float2 iTexCoord : ATTRIB3,
        float4 iTangent : ATTRIB4,
        out float2 oTexCoord : TEXCOORD0,
        out float4 oColor : COLOR0,
		out float4 oPos : OUTPOSITION
    ) 
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);
    oTexCoord = iTexCoord;
	oColor = iColor;
}

float4 PS(float2 iTexCoord : TEXCOORD0, float4 iColor : COLOR0) : SV_TARGET
{ 
    float4 diffColor = iColor;//cMatDiffColor;

    //#ifdef DIFFMAP
        float4 diffInput = DiffMap.Sample(DiffMap_sampler, iTexCoord);
        #ifdef ALPHAMASK
            if (diffInput.a < 0.5)
                discard;
        #endif
        return diffColor * diffInput;
   // #endif
    #ifdef ALPHAMAP
        float alphaInput = Sample2D(DiffMap, iTexCoord).a;
        return float4(diffColor.rgb, diffColor.a * alphaInput);
    #endif

}
