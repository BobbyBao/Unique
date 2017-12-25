
cbuffer Constants
{
    float4x4 g_WorldViewProj;
	float4x4 g_WorldNorm;
    float3 g_LightDir;
    float g_LightCoeff;
};

struct PSInput 
{ 
    float4 Pos : SV_POSITION; 
};

PSInput VS(float3 pos : ATTRIB0, float3 norm : ATTRIB1) 
{
    PSInput ps; 
    ps.Pos = mul( float4(pos,1), g_WorldViewProj);
    return ps;
}

float4 PS(PSInput input) : SV_TARGET
{
    return float4(1, 1, 1, 1); 
}
