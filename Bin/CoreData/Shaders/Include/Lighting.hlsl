#pragma warning(disable:3571)

#ifdef COMPILEVS

#ifdef SHADOW

#ifdef DIRLIGHT
    #define NUMCASCADES 4
#else
    #define NUMCASCADES 1
#endif

void GetShadowPos(float4 projWorldPos, float3 normal, out float4 shadowPos[NUMCASCADES])
{
    // Shadow projection: transform from world space to shadow space
    #ifdef NORMALOFFSET
        #ifdef DIRLIGHT
            float cosAngle = saturate(1.0 - dot(normal, LightDir));
        #else
            float cosAngle = saturate(1.0 - dot(normal, normalize(LightPos.xyz - projWorldPos.xyz)));
        #endif

        #if defined(DIRLIGHT)
            shadowPos[0] = mul(float4(projWorldPos.xyz + cosAngle * NormalOffsetScale.x * normal, 1.0), LightMatrices[0]);
            shadowPos[1] = mul(float4(projWorldPos.xyz + cosAngle * NormalOffsetScale.y * normal, 1.0), LightMatrices[1]);
            shadowPos[2] = mul(float4(projWorldPos.xyz + cosAngle * NormalOffsetScale.z * normal, 1.0), LightMatrices[2]);
            shadowPos[3] = mul(float4(projWorldPos.xyz + cosAngle * NormalOffsetScale.w * normal, 1.0), LightMatrices[3]);
        #elif defined(SPOTLIGHT)
            shadowPos[0] = mul(float4(projWorldPos.xyz + cosAngle * NormalOffsetScale.x * normal, 1.0), LightMatrices[1]);
        #else
            shadowPos[0] = float4(projWorldPos.xyz + cosAngle * NormalOffsetScale.x * normal - LightPos.xyz, 0.0);
        #endif
    #else
        #if defined(DIRLIGHT)
            shadowPos[0] = mul(projWorldPos, LightMatrices[0]);
            shadowPos[1] = mul(projWorldPos, LightMatrices[1]);
            shadowPos[2] = mul(projWorldPos, LightMatrices[2]);
            shadowPos[3] = mul(projWorldPos, LightMatrices[3]);
        #elif defined(SPOTLIGHT)
            shadowPos[0] = mul(projWorldPos, LightMatrices[1]);
        #else
            shadowPos[0] = float4(projWorldPos.xyz - LightPos.xyz, 0.0);
        #endif
    #endif
}
#endif
#endif

#ifdef COMPILEPS
float GetDiffuse(float3 normal, float3 worldPos, out float3 lightDir)
{
    #ifdef DIRLIGHT
        lightDir = LightDirPS;
        #ifdef TRANSLUCENT
            return abs(dot(normal, lightDir));
        #else
            return saturate(dot(normal, lightDir));
        #endif
    #else
        float3 lightVec = (LightPosPS.xyz - worldPos) * LightPosPS.w;
        float lightDist = length(lightVec);
        lightDir = lightVec / lightDist;
        #ifdef TRANSLUCENT
            return abs(dot(normal, lightDir)) * Sample2D(LightRampMap, float2(lightDist, 0.0)).r;
        #else
            return saturate(dot(normal, lightDir)) * Sample2D(LightRampMap, float2(lightDist, 0.0)).r;
        #endif
    #endif
}

float GetAtten(float3 normal, float3 worldPos, out float3 lightDir)
{
    lightDir = LightDirPS;
    return saturate(dot(normal, lightDir));
    
}

float GetAttenPoint(float3 normal, float3 worldPos, out float3 lightDir)
{
    float3 lightVec = (LightPosPS.xyz - worldPos) * LightPosPS.w;
    float lightDist = length(lightVec);
    float falloff = pow(saturate(1.0 - pow(lightDist / 1.0, 4.0)), 2.0) * 3.14159265358979323846 / (4 * 3.14159265358979323846)*(pow(lightDist, 2.0) + 1.0);
    lightDir = lightVec / lightDist;
    return saturate(dot(normal, lightDir)) * falloff;

}

float GetAttenSpot(float3 normal, float3 worldPos, out float3 lightDir)
{
    float3 lightVec = (LightPosPS.xyz - worldPos) * LightPosPS.w;
    float lightDist = length(lightVec);
    float falloff = pow(saturate(1.0 - pow(lightDist / 1.0, 4.0)), 2.0) / (pow(lightDist, 2.0) + 1.0);

    lightDir = lightVec / lightDist;
    return saturate(dot(normal, lightDir)) * falloff;

}


float GetDiffuseVolumetric(float3 worldPos)
{
    #ifdef DIRLIGHT
        return 1.0;
    #else
        float3 lightVec = (LightPosPS.xyz - worldPos) * LightPosPS.w;
        float lightDist = length(lightVec);
        return Sample2D(LightRampMap, float2(lightDist, 0.0)).r;
    #endif
}

float GetSpecular(float3 normal, float3 eyeVec, float3 lightDir, float specularPower)
{
    float3 halfVec = normalize(normalize(eyeVec) + lightDir);
    return saturate(pow(dot(normal, halfVec), specularPower));
}

float GetIntensity(float3 color)
{
    return dot(color, float3(0.299, 0.587, 0.114));
}

#ifdef SHADOW

#ifdef DIRLIGHT
    #define NUMCASCADES 4
#else
    #define NUMCASCADES 1
#endif

#ifdef VSM_SHADOW
float ReduceLightBleeding(float min, float p_max)  
{  
    return clamp((p_max - min) / (1.0 - min), 0.0, 1.0);  
}

float Chebyshev(float2 Moments, float depth)  
{  
    //One-tailed inequality valid if depth > Moments.x  
    float p = float(depth <= Moments.x);  
    //Compute variance.
    float Variance = Moments.y - (Moments.x * Moments.x); 

    float minVariance = cVSMShadowParams.x;
    Variance = max(Variance, minVariance);  
    //Compute probabilistic upper bound.  
    float d = depth - Moments.x;  
    float p_max = Variance / (Variance + d*d); 
    // Prevent light bleeding
    p_max = ReduceLightBleeding(cVSMShadowParams.y, p_max);

    return max(p, p_max);
}
#endif

float GetShadow(float4 shadowPos)
{
    #if defined(SIMPLE_SHADOW)
        // Take one sample
        #ifdef D3D11
            shadowPos.xyz /= shadowPos.w;
        #endif
        float inLight = SampleShadow(ShadowMap, shadowPos).r;
        #ifndef SHADOWCMP
            return cShadowIntensity.y + cShadowIntensity.x * inLight;
        #else
            #ifndef POINTLIGHT
                return cShadowIntensity.y + cShadowIntensity.x * (inLight * shadowPos.w > shadowPos.z);
            #else
                return cShadowIntensity.y + cShadowIntensity.x * (inLight > shadowPos.z);
            #endif
        #endif
    
    #elif defined(PCF_SHADOW)
        // Take four samples and average them
        // Note: in case of sampling a point light cube shadow, we optimize out the w divide as it has already been performed
        #ifdef D3D11
            shadowPos.xyz /= shadowPos.w;
        #endif
        #if !defined(POINTLIGHT) && !defined(D3D11)
            float2 offsets = cShadowMapInvSize * shadowPos.w;
        #else
            float2 offsets = cShadowMapInvSize;
        #endif
        float4 shadowPos2 = float4(shadowPos.x + offsets.x, shadowPos.yzw);
        float4 shadowPos3 = float4(shadowPos.x, shadowPos.y + offsets.y, shadowPos.zw);
        float4 shadowPos4 = float4(shadowPos.xy + offsets.xy, shadowPos.zw);

        float4 inLight = float4(
            SampleShadow(ShadowMap, shadowPos).r,
            SampleShadow(ShadowMap, shadowPos2).r,
            SampleShadow(ShadowMap, shadowPos3).r,
            SampleShadow(ShadowMap, shadowPos4).r
        );
        #ifndef SHADOWCMP
            return cShadowIntensity.y + dot(inLight, cShadowIntensity.x);
        #else
            #ifndef POINTLIGHT
                return cShadowIntensity.y + dot(inLight * shadowPos.w > shadowPos.z, cShadowIntensity.x);
            #else
                return cShadowIntensity.y + dot(inLight > shadowPos.z, cShadowIntensity.x);
            #endif
        #endif
    
    #elif defined(VSM_SHADOW)
        float2 samples = Sample2D(ShadowMap, shadowPos.xy / shadowPos.w).rg;
        return cShadowIntensity.y + cShadowIntensity.x * Chebyshev(samples, shadowPos.z/shadowPos.w);
    #endif
}

#ifdef POINTLIGHT
float GetPointShadow(float3 lightVec)
{
    float3 axis = SampleCube(FaceSelectCubeMap, lightVec).rgb;
    float depth = abs(dot(lightVec, axis));

    // Expand the maximum component of the light vector to get full 0.0 - 1.0 UV range from the cube map,
    // and to avoid sampling across faces. Some GPU's filter across faces, while others do not, and in this
    // case filtering across faces is wrong
    const float factor = 1.0 / 256.0;
    lightVec += factor * axis * lightVec;

    // Read the 2D UV coordinates, adjust according to shadow map size and add face offset
    float4 indirectPos = SampleCube(IndirectionCubeMap, lightVec);
    indirectPos.xy *= cShadowCubeAdjust.xy;
    indirectPos.xy += float2(cShadowCubeAdjust.z + indirectPos.z * 0.5, cShadowCubeAdjust.w + indirectPos.w);

    float4 shadowPos = float4(indirectPos.xy, cShadowDepthFade.x + cShadowDepthFade.y / depth, 1.0);
    return GetShadow(shadowPos);
}
#endif

#ifdef DIRLIGHT
float GetDirShadowFade(float inLight, float depth)
{
    return saturate(inLight + saturate((depth - cShadowDepthFade.z) * cShadowDepthFade.w));
}

float GetDirShadow(const float4 iShadowPos[NUMCASCADES], float depth)
{
    float4 shadowPos;

    if (depth < cShadowSplits.x)
        shadowPos = iShadowPos[0];
    else if (depth < cShadowSplits.y)
        shadowPos = iShadowPos[1];
    else if (depth < cShadowSplits.z)
        shadowPos = iShadowPos[2];
    else
        shadowPos = iShadowPos[3];

    return GetDirShadowFade(GetShadow(shadowPos), depth);
}

float GetDirShadowDeferred(float4 projWorldPos, float3 normal, float depth)
{
    float4 shadowPos;

    #ifdef NORMALOFFSET
        float cosAngle = saturate(1.0 - dot(normal, LightDirPS));
        if (depth < cShadowSplits.x)
            shadowPos = mul(float4(projWorldPos.xyz + cosAngle * NormalOffsetScalePS.x * normal, 1.0), LightMatricesPS[0]);
        else if (depth < cShadowSplits.y)
            shadowPos = mul(float4(projWorldPos.xyz + cosAngle * NormalOffsetScalePS.y * normal, 1.0), LightMatricesPS[1]);
        else if (depth < cShadowSplits.z)
            shadowPos = mul(float4(projWorldPos.xyz + cosAngle * NormalOffsetScalePS.z * normal, 1.0), LightMatricesPS[2]);
        else
            shadowPos = mul(float4(projWorldPos.xyz + cosAngle * NormalOffsetScalePS.w * normal, 1.0), LightMatricesPS[3]);
    #else
        if (depth < cShadowSplits.x)
            shadowPos = mul(projWorldPos, LightMatricesPS[0]);
        else if (depth < cShadowSplits.y)
            shadowPos = mul(projWorldPos, LightMatricesPS[1]);
        else if (depth < cShadowSplits.z)
            shadowPos = mul(projWorldPos, LightMatricesPS[2]);
        else
            shadowPos = mul(projWorldPos, LightMatricesPS[3]);
    #endif
    
    return GetDirShadowFade(GetShadow(shadowPos), depth);
}
#endif

float GetShadow(float4 iShadowPos[NUMCASCADES], float depth)
{
    #if defined(DIRLIGHT)
        return GetDirShadow(iShadowPos, depth);
    #elif defined(SPOTLIGHT)
        return GetShadow(iShadowPos[0]);
    #else
        return GetPointShadow(iShadowPos[0].xyz);
    #endif
}

float GetShadowDeferred(float4 projWorldPos, float3 normal, float depth)
{
    #ifdef DIRLIGHT
        return GetDirShadowDeferred(projWorldPos, normal, depth);
    #else
        #ifdef NORMALOFFSET
            float cosAngle = saturate(1.0 - dot(normal, normalize(LightPosPS.xyz - projWorldPos.xyz)));
            projWorldPos.xyz += cosAngle * NormalOffsetScalePS.x * normal;
        #endif

        #ifdef SPOTLIGHT
            float4 shadowPos = mul(projWorldPos, LightMatricesPS[1]);
            return GetShadow(shadowPos);
        #else
            float3 shadowPos = projWorldPos.xyz - LightPosPS.xyz;
            return GetPointShadow(shadowPos);
        #endif
    #endif
}
#endif
#endif