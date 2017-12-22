

Shader "Basic"
{
	Pass base
	{
		DepthState
		{
			DepthEnable true
			DepthWriteEnable true
		}
		
		RasterizerState 
		{
			FillMode SOLID
			CullMode NONE
		}

		InputLayout
		{
			NumComponents 3
			ValueType FLOAT32
			IsNormalized false
		}
		
		InputLayout
		{
			NumComponents 3
			ValueType FLOAT32
			IsNormalized false
		}
		
		InputLayout
		{
			NumComponents 4
			ValueType UINT8
			IsNormalized true
		}
	
		VertexShader
		{ 
			EntryPoint "VS" 
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Default.hlsl"
		}
		
		PixelShader
		{
			EntryPoint "PS"
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Default.hlsl"
		}
	}
}