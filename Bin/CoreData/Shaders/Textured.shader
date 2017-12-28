

Shader "Basic"
{
	Uniforms
	{
	}

	TextureSlots
	{
	}


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
			Source "Textured.hlsl"
		}
		
		PixelShader
		{
			EntryPoint "PS"
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Textured.hlsl"
		}
	}
}