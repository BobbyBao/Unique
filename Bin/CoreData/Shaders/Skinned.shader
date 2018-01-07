

Shader "Skinned"
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
				IsNormalized false
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
			
			LayoutElement
			{
				NumComponents 4
				ValueType FLOAT32
				IsNormalized false
			}
			
			LayoutElement
			{
				NumComponents 4
				ValueType UINT8
				IsNormalized false
			}
		}
	
		VertexShader
		{ 
			EntryPoint "VS" 
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Skinned.hlsl"
		}
		
		PixelShader
		{
			EntryPoint "PS"
			ShaderProfile "DX_4_0"
			Defines ""
			Source "Skinned.hlsl"
		}
	}
}