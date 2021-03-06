#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"


void VS(
		float3 iPos : ATTRIB0, 
        float3 iNormal : ATTRIB1,
        float2 iTexCoord : ATTRIB2,
        float4 iTangent : ATTRIB3,
        float4 iBlendWeights : ATTRIB4,
        int4 iBlendIndices : ATTRIB5,
        out float2 oTexCoord : TEXCOORD0,
        out float4 oColor: TEXCOORD1,
		out float4 oPos : OUTPOSITION
    ) 
{
    float4x3 modelMatrix =  iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix).xyz;
    oPos = GetClipPos(worldPos);
    oTexCoord = iTexCoord;
}


float4 PS(float2 iTexCoord : TEXCOORD0) : SV_TARGET
{
    float4 diffColor = float4(1,1,1,1);//cMatDiffColor;

    //#ifdef DIFFMAP
        float4 diffInput = Sample2D(DiffMap, iTexCoord);
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
