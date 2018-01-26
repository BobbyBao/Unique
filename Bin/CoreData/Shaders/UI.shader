

Shader "UI"
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
			DepthEnable false
			DepthWriteEnable false
		}
		
		RasterizerState 
		{
			CullMode NONE
			AntialiasedLineEnable true
			ScissorEnable true
		}
		
		BlendState
		{
			RenderTargets[0]
			{
				BlendEnable true
				SrcBlend SRC_ALPHA
				DestBlend INV_SRC_ALPHA
			}
		}
		
		InputLayout
		{
			LayoutElement
			{
				NumComponents 2
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
				ValueType UINT8
				IsNormalized true
			}
		}
	
		VertexShader
		{ 
			EntryPoint "VS" 
			ShaderProfile "DX_4_0"
			Defines ""
			Source "UI.hlsl"
		}
		
		PixelShader
		{
			EntryPoint "PS"
			ShaderProfile "DX_4_0"
			Defines ""
			Source "UI.hlsl"
		}
	}
}