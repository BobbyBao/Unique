#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"
#include "Lighting.hlsl"
#include "Fog.hlsl"

void VS(
		float4 iPos : ATTRIB0, 
        float3 iNormal : ATTRIB1,
        float2 iTexCoord : ATTRIB2,
        float4 iTangent : ATTRIB3,
		float4x3 iModelInstance : ATTRIB4,
        out float2 oTexCoord : TEXCOORD0,
        out float3 oNormal : TEXCOORD1,
        out float4 oWorldPos : TEXCOORD2,
        out float4 oTangent : TEXCOORD3,
		out float4 oPos : OUTPOSITION
    ) 
{
    float4x3 modelMatrix = iModelMatrix;
    float3 worldPos = GetWorldPos(modelMatrix);
    oPos = GetClipPos(worldPos);

    oWorldPos = float4(worldPos, GetDepth(oPos));
      
    #if defined(D3D11) && defined(CLIPPLANE)
        oClip = dot(oPos, ClipPlane);
    #endif

    #ifdef NORMALMAP
        float4 tangent = GetWorldTangent(modelMatrix);
        float3 bitangent = cross(tangent.xyz, oNormal) * tangent.w;
        oTexCoord = float4(GetTexCoord(iTexCoord), bitangent.xy);
        oTangent = float4(tangent.xyz, bitangent.z);
    #else
        oTexCoord = /*GetTexCoord(*/iTexCoord/*)*/;
    #endif
}

void PS(
        float2 iTexCoord : TEXCOORD0,
        float3 iNormal : TEXCOORD1,
        float4 iWorldPos : TEXCOORD2, 
        float4 iTangent : TEXCOORD3,
 
        out float4 oColor : OUTCOLOR0)
{ 
    // Get material diffuse albedo
    float4 diffInput = Sample2D(DiffMap, iTexCoord);
    #ifdef ALPHAMASK
        if (diffInput.a < 0.5)
            discard;
    #endif

    float4 diffColor = /*MatDiffColor **/ diffInput;

    // Get material specular albedo
    #ifdef SPECMAP
        float3 specColor = MatSpecColor.rgb * Sample2D(SpecMap, iTexCoord.xy).rgb;
    #else
        float3 specColor = MatSpecColor.rgb;
    #endif

    // Get normal
    #ifdef NORMALMAP
        float3x3 tbn = float3x3(iTangent.xyz, float3(iTexCoord.zw, iTangent.w), iNormal);
        float3 normal = normalize(mul(DecodeNormal(Sample2D(NormalMap, iTexCoord.xy)), tbn));
    #else
        float3 normal = normalize(iNormal);
    #endif

    // Get fog factor
    #ifdef HEIGHTFOG
        float fogFactor = GetHeightFogFactor(iWorldPos.w, iWorldPos.y);
    #else
        float fogFactor = GetFogFactor(iWorldPos.w);
    #endif

    // Per-pixel forward lighting
    float3 lightDir;
    float3 lightColor;
    float3 finalColor;

    float diff = GetDiffuse(normal, iWorldPos.xyz, lightDir);

    #ifdef SHADOW
        diff *= GetShadow(iShadowPos, iWorldPos.w);
    #endif

    #if defined(SPOTLIGHT)
        lightColor = iSpotPos.w > 0.0 ? Sample2DProj(LightSpotMap, iSpotPos).rgb * LightColor.rgb : 0.0;
    #elif defined(CUBEMASK)
        lightColor = SampleCube(LightCubeMap, iCubeMaskVec).rgb * LightColor.rgb;
    #else
        lightColor = LightColor.rgb;
    #endif

    #ifdef SPECULAR
        float spec = GetSpecular(normal, CameraPosPS - iWorldPos.xyz, lightDir, MatSpecColor.a);
        finalColor = diff * lightColor * (diffColor.rgb + spec * specColor * LightColor.a);
    #else
        finalColor = /*diff * lightColor **/ diffColor.rgb;
    #endif

    #ifdef AMBIENT
        finalColor += AmbientColor.rgb * diffColor.rgb;
        finalColor += MatEmissiveColor;
        oColor = float4(GetFog(finalColor, fogFactor), diffColor.a);
    #else
        oColor = float4(finalColor, diffColor.a);//float4(GetLitFog(finalColor, fogFactor), diffColor.a);
    #endif


}
