

Shader "Normal"
{
	Properties
	{

		TextureSlot DiffMap
		{
			Texture ""
		}
	
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
				NumComponents 4
				ValueType UINT8
				IsNormalized true
			}
			
			LayoutElement
			{
				NumComponents 2
				ValueType FLOAT32
				IsNormalized false
			}
			
			LayoutElement
			{
				NumComponents 4
				ValueType FLOAT32
				IsNormalized false
			}
			
		}
	
		VertexShader
		{ 
			EntryPoint "VS" 
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Normal.hlsl"
		}
		
		PixelShader
		{
			EntryPoint "PS"
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Normal.hlsl"
		}
	}
}