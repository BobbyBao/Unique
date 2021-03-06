

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
			CullMode NONE
		}
		
		InputLayout
		{
			LayoutElement
			{
				NumComponents 3
				ValueType FLOAT32
				IsNormalized false
			}

			LayoutElement
			{
				NumComponents 4
				ValueType UINT8
				IsNormalized true
			}
			
		}
	
		VertexShader
		{ 
			EntryPoint "VS" 
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Debug.hlsl"
		}
		
		PixelShader
		{
			EntryPoint "PS"
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Debug.hlsl"
		}
	}
}