

Shader "Normal"
{
	Pass base
	{
		Properties
		{

			TextureSlot DiffMap
			{
				Texture ""
			}
		
		}


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
			
			LayoutElement
			{
				NumComponents 2
				ValueType FLOAT32
			}
			
			LayoutElement
			{
				NumComponents 4
				ValueType FLOAT32
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