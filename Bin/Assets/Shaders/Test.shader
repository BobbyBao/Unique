
Shader("Default"):
{
    Pass("Main"):
	{
		DepthState: 
		{
			DepthEnable: true
			DepthWriteEnable: true
		}
		
		InputLayout:
		[
			{
				InputIndex: 0
				NumComponents: 3
				ValueType: FLOAT32
				IsNormalized: false
			}
			{
				InputIndex: 1
				NumComponents: 3
				ValueType: FLOAT32
				IsNormalized: false
			}
			{
				InputIndex: 2
				NumComponents: 4
				ValueType: UINT8
				IsNormalized: true
			}
		]
		
		VertexShader:
		{ 
			EntryPoint: "VS" 
			ShaderProfile: "DX_4_0"
			Defines: ""
			Source: "Default.hlsl"
		}
		
		PixelShader:
		{
			EntryPoint: "PS"
			ShaderProfile: "DX_4_0"
			Defines: ""
			Source: "Default.hlsl"
		}
		
	}
   

}