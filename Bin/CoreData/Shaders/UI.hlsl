Texture2D DiffMap;
SamplerState DiffMap_sampler;


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

PS_INPUT VS(VS_INPUT input)
{
  PS_INPUT output;
  output.pos = mul(UIProj, float4(input.pos.xy, 0.f, 1.f));
  output.col = input.col;
  output.uv  = input.uv;
  return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
  return input.col * DiffMap.Sample(DiffMap_sampler, input.uv);
}