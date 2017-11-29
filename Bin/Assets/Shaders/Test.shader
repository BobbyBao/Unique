{
    "Type": "Shader",
    "Name": "Test",
    "ShaderDefines": "",
    "ShaderPasses": [
        {
            "Type": "ShaderPass",
            "Name": "",
            "DepthState": {
                "TestEnabled": false,
                "WriteEnabled": false,
                "CompareOp": "Less"
            },
            "ShaderStages": [
                {
                    "Name": "VertexShader",
                    "FileName": "Assets/shader.hlsl",
                    "EntryPoint": "VS",
                    "Target": "vs_5_0"
                },
                {
                    "Name": "FragmentShader",
                    "FileName": "Assets/shader.hlsl",
                    "EntryPoint": "PS",
                    "Target": "ps_5_0"
                }
            ]
        }
    ]
}