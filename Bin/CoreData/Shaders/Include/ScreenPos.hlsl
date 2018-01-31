#ifdef COMPILEVS
float3x3 GetCameraRot()
{
    return float3x3(ViewInv[0][0], ViewInv[0][1], ViewInv[0][2],
        ViewInv[1][0], ViewInv[1][1], ViewInv[1][2],
        ViewInv[2][0], ViewInv[2][1], ViewInv[2][2]);
}

float4 GetScreenPos(float4 clipPos)
{
    return float4(
        clipPos.x * GBufferOffsets.z + GBufferOffsets.x * clipPos.w,
        -clipPos.y * GBufferOffsets.w + GBufferOffsets.y * clipPos.w,
        0.0,
        clipPos.w);
}

float2 GetScreenPosPreDiv(float4 clipPos)
{
    return float2(
        clipPos.x / clipPos.w * GBufferOffsets.z + GBufferOffsets.x,
        -clipPos.y / clipPos.w * GBufferOffsets.w + GBufferOffsets.y);
}

float2 GetQuadTexCoord(float4 clipPos)
{
    return float2(
        clipPos.x / clipPos.w * 0.5 + 0.5,
        -clipPos.y / clipPos.w * 0.5 + 0.5);
}

float2 GetQuadTexCoordNoFlip(float3 worldPos)
{
    return float2(
        worldPos.x * 0.5 + 0.5,
        -worldPos.y * 0.5 + 0.5);
}

float3 GetFarRay(float4 clipPos)
{
    float3 viewRay = float3(
        clipPos.x / clipPos.w * FrustumSize.x,
        clipPos.y / clipPos.w * FrustumSize.y,
        FrustumSize.z);

    return mul(viewRay, GetCameraRot());
}

float3 GetNearRay(float4 clipPos)
{
    float3 viewRay = float3(
        clipPos.x / clipPos.w * FrustumSize.x,
        clipPos.y / clipPos.w * FrustumSize.y,
        0.0);

    return mul(viewRay, GetCameraRot()) * DepthMode.z;
}
#endif
