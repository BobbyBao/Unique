Texture2D DiffMap;
SamplerState DiffMap_sampler;


cbuffer CameraVS
{  
	float4x3 cView;
    float4x3 cViewInv;
    float4x4 cViewProj;
    float3 cCameraPos;
    float cNearClip;
    float cFarClip;
    float3 cFrustumSize;
    float4 cDepthMode;
    float4 cGBufferOffsets;
    float4 cClipPlane;
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

PS_INPUT vs(VS_INPUT input)
{
  PS_INPUT output;
  output.pos = mul(float4(input.pos.xy, 0.f, 1.f), cViewProj);
  output.col = input.col;
  output.uv  = input.uv;
  return output;
}

float4 ps(PS_INPUT input) : SV_Target
{
  return input.col * DiffMap.Sample(DiffMap_sampler, input.uv);
}