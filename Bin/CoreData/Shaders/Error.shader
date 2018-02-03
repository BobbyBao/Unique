

Shader "Error"
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
			//FillMode WIREFRAME
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
				NumComponents 3
				ValueType FLOAT32
				IsNormalized false
			}
			
			LayoutElement
			{
				NumComponents 2
				ValueType FLOAT32
				IsNormalized false
			}
		}
	
		VertexShader
		{ 
			EntryPoint "VS" 
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Error.hlsl"
		}
		
		PixelShader
		{
			EntryPoint "PS"
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Error.hlsl"
		}
	}
}