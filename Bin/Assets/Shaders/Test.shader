
Shader("Default"):
{
    Pass("Main"):
	{
		DepthState: 
		{
			TestEnabled: false
			WriteEnabled: true
			CompareOp: Less
		}
		
		ShaderDefines: ""
		
		VertexShader:
		{ 
			EntryPoint: "VS" 
			Target: "vs_4_0"
		}
		
		PixelShader:
		{
			EntryPoint: "PS"
			Target: "ps_4_0"
		}
		
		Source:	'''


		// HLSL texturing shader

		Texture2D colorMap : register(t0);
		SamplerState samplerState : register(s0);

		struct InputVS
		{
			float2 position : POSITION;
			float2 texCoord : TEXCOORD;
		};

		struct OutputVS
		{
			float4 position : SV_Position;
			float2 texCoord : TEXCOORD;
		};


		// VERTEX SHADER

		OutputVS VS(InputVS inp)
		{
			OutputVS outp;
			outp.position = float4(inp.position, 0, 1);
			outp.texCoord = inp.texCoord;
			return outp;
		}


		// PIXEL SHADER

		float4 PS(OutputVS inp) : SV_Target
		{
			return colorMap.Sample(samplerState, inp.texCoord);
		}


		'''
	}

}