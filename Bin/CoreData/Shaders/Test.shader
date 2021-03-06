
Shader "Default"
{
    Pass "base"
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
			LayoutElement
			{
				NumComponents 3
				ValueType FLOAT32
			}
			
			LayoutElement
			{
				NumComponents 3
				ValueType FLOAT32
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
			Source "Test.hlsl"
		}
		
		PixelShader
		{
			EntryPoint "PS"
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Test.hlsl"
		}
		
	}
   

}