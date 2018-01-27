#include "Uniforms.hlsl"
#include "Samplers.hlsl"

cbuffer UIVS
{  
    float4x4 UIProj;
}

struct VS_INPUT
{
  float2 pos : ATTRIB0;
  float2 uv  : ATTRIB1;
  float4 col : ATTRIB2;
};

struct PS_INPUT
{
  float4 pos : SV_POSITION;
  float4 col : COLOR0;
  float2 uv  : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT vs_input)
{
  PS_INPUT o;
  o.pos = mul(float4(vs_input.pos.xy, 0.f, 1.f), UIProj);
  o.col = vs_input.col;
  o.uv  = vs_input.uv;
  return o;
}

float4 PS(PS_INPUT ps_input) : SV_Target
{
  return ps_input.col * Sample2D(DiffMap, ps_input.uv);
}