#ifdef COMPILEVS

#ifdef D3D11
#define OUTPOSITION SV_POSITION
#else
#define OUTPOSITION POSITION
#endif

#ifdef SKINNED

float4x3 GetSkinMatrix(float4 blendWeights, int4 blendIndices)
{
    return SkinMatrices[blendIndices.x] * blendWeights.x +
        SkinMatrices[blendIndices.y] * blendWeights.y +
        SkinMatrices[blendIndices.z] * blendWeights.z +
        SkinMatrices[blendIndices.w] * blendWeights.w;
}

#endif

float2 GetTexCoord(float2 iTexCoord)
{
    return float2(dot(iTexCoord, UOffset.xy) + UOffset.w, dot(iTexCoord, VOffset.xy) + VOffset.w);
};

float4 GetClipPos(float3 worldPos)
{
    return mul(float4(worldPos, 1.0), ViewProj);
}

//float GetZonePos(float3 worldPos)
//{
//    return saturate(mul(float4(worldPos, 1.0), Zone).z);
//}

float GetDepth(float4 clipPos)
{
    return dot(clipPos.zw, DepthMode.zw);
}

#ifdef BILLBOARD
float3 GetBillboardPos(float4 iPos, float2 iSize, float4x3 modelMatrix)
{
    return mul(iPos, modelMatrix) + mul(float3(iSize.x, iSize.y, 0.0), BillboardRot);
}

float3 GetBillboardNormal()
{
    return float3(-BillboardRot[2][0], -BillboardRot[2][1], -BillboardRot[2][2]);
}
#endif

#ifdef DIRBILLBOARD
float3x3 GetFaceCameraRotation(float3 position, float3 direction)
{
    float3 cameraDir = normalize(position - CameraPos);
    float3 front = normalize(direction);
    float3 right = normalize(cross(front, cameraDir));
    float3 up = normalize(cross(front, right));

    return float3x3(
        right.x, right.y, right.z,
        up.x, up.y, up.z,
        front.x, front.y, front.z
    );
}

float3 GetBillboardPos(float4 iPos, float2 iSize, float3 iDirection, float4x3 modelMatrix)
{
    float3 worldPos = mul(iPos, modelMatrix);
    return worldPos + mul(float3(iSize.x, 0.0, iSize.y), GetFaceCameraRotation(worldPos, iDirection));
}

float3 GetBillboardNormal(float4 iPos, float3 iDirection, float4x3 modelMatrix)
{
    float3 worldPos = mul(iPos, modelMatrix);
    return mul(float3(0.0, 1.0, 0.0), GetFaceCameraRotation(worldPos, iDirection));
}
#endif

#ifdef TRAILFACECAM
float3 GetTrailPos(float3 iPos, float3 iFront, float iScale, float4x3 modelMatrix)
{
    float3 up = normalize(CameraPos - iPos.xyz);
    float3 left = normalize(cross(iFront, up));
    return (mul(float4((iPos.xyz + left * iScale), 1.0), modelMatrix)).xyz;
}

float3 GetTrailNormal(float3 iPos)
{
    return normalize(CameraPos - iPos.xyz);
}
#endif

#ifdef TRAILBONE
float3 GetTrailPos(float3 iPos, float3 iParentPos, float iScale, float4x3 modelMatrix)
{
    float3 right = iParentPos - iPos.xyz;
    return (mul(float4((iPos.xyz + right * iScale), 1.0), modelMatrix)).xyz;
}

float3 GetTrailNormal(float3 iPos, float3 iParentPos, float3 iForward)
{
    float3 left = normalize(iPos.xyz - iParentPos);
    float3 up = -normalize(cross(normalize(iForward), left));
    return up;
}
#endif

#if defined(SKINNED)
    #define iModelMatrix GetSkinMatrix(iBlendWeights, iBlendIndices)
#elif defined(INSTANCED)
    #define iModelMatrix iModelInstance
#else
    #define iModelMatrix Model
#endif

#if defined(BILLBOARD)
    #define GetWorldPos(modelMatrix) GetBillboardPos(float4(iPos, 1), iSize, modelMatrix)
#elif defined(DIRBILLBOARD)
    #define GetWorldPos(modelMatrix) GetBillboardPos(float4(iPos, 1), iSize, iNormal, modelMatrix)
#elif defined(TRAILFACECAM)
    #define GetWorldPos(modelMatrix) GetTrailPos(iPos, iTangent.xyz, iTangent.w, modelMatrix)
#elif defined(TRAILBONE)
    #define GetWorldPos(modelMatrix) GetTrailPos(iPos, iTangent.xyz, iTangent.w, modelMatrix)
#else
    #define GetWorldPos(modelMatrix) mul(float4(iPos, 1), modelMatrix)
#endif

#if defined(BILLBOARD)
    #define GetWorldNormal(modelMatrix) GetBillboardNormal()
#elif defined(DIRBILLBOARD)
    #define GetWorldNormal(modelMatrix) GetBillboardNormal(iPos, iNormal, modelMatrix)
#elif defined(TRAILFACECAM)
    #define GetWorldNormal(modelMatrix) GetTrailNormal(iPos)
#elif defined(TRAILBONE)
    #define GetWorldNormal(modelMatrix) GetTrailNormal(float4(iPos, 1), iTangent.xyz, iNormal)
#else
    #define GetWorldNormal(modelMatrix) normalize(mul(iNormal, (float3x3)modelMatrix))
#endif

#if defined(BILLBOARD)
    #define GetWorldTangent(modelMatrix) float4(normalize(mul(float3(1.0, 0.0, 0.0), BillboardRot)), 1.0)
#elif defined(DIRBILLBOARD)
    #define GetWorldTangent(modelMatrix) float4(normalize(mul(float3(1.0, 0.0, 0.0), (float3x3)modelMatrix)), 1.0)
#else
    #define GetWorldTangent(modelMatrix) float4(normalize(mul(iTangent.xyz, (float3x3)modelMatrix)), iTangent.w)
#endif

#endif

#ifdef COMPILEPS

#ifdef D3D11
#define OUTCOLOR0 SV_TARGET
#define OUTCOLOR1 SV_TARGET1
#define OUTCOLOR2 SV_TARGET2
#define OUTCOLOR3 SV_TARGET3
#else
#define OUTCOLOR0 COLOR0
#define OUTCOLOR1 COLOR1
#define OUTCOLOR2 COLOR2
#define OUTCOLOR3 COLOR3
#endif

#endif

