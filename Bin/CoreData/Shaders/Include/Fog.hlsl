#ifdef COMPILEPS
float3 GetFog(float3 color, float fogFactor)
{
    return lerp(FogColor, color, fogFactor);
}

float3 GetLitFog(float3 color, float fogFactor)
{
    return color * fogFactor;
}

float GetFogFactor(float depth)
{
    return saturate((FogParams.x - depth) * FogParams.y);
}

float GetHeightFogFactor(float depth, float height)
{
    float fogFactor = GetFogFactor(depth);
    float heightFogFactor = (height - FogParams.z) * FogParams.w;
    heightFogFactor = 1.0 - saturate(exp(-(heightFogFactor * heightFogFactor)));
    return min(heightFogFactor, fogFactor);
}
#endif
