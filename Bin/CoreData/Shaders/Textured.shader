

Shader "Textured"
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
				ValueType FLOAT32
				NumComponents 3
			}

			LayoutElement
			{
				ValueType FLOAT32
				NumComponents 3
			}
			
			LayoutElement
			{
				ValueType FLOAT32
				NumComponents 2
			}

			LayoutElement
			{
				ValueType FLOAT32
				NumComponents 4
			}
			
			LayoutElement
			{
				ValueType FLOAT32
				NumComponents 4
				BufferSlot 1
				Frequency PER_INSTANCE
			}
			
			LayoutElement
			{
				ValueType FLOAT32
				NumComponents 4
				BufferSlot 1
				Frequency PER_INSTANCE
			}
			
			LayoutElement
			{
				ValueType FLOAT32
				NumComponents 4
				BufferSlot 1
				Frequency PER_INSTANCE
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