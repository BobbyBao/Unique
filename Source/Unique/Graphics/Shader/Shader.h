#pragma once
#include "../../Resource/Resource.h"
#include "../GraphicsDefs.h"
#include "ShaderVariation.h"

namespace Unique
{
	class Shader;

	class Pass : public Object
	{
		uRTTI(Pass, Object)
	public:
		Pass();
		~Pass();

		uint GetMask(Shader* shader, const String& defs);

		ShaderInstance* GetInstance(Shader* shader, const String & defs);

		ShaderInstance* GetInstance(Shader* shader, unsigned defMask);

		bool Prepare();
	private:
		StringID				name_;
		
		DepthState				depthState_;
		StencilState			stencilState_;
		RasterizerState			rasterizerState_;
		BlendState				blendState_;
				
		ShaderStage				vertexShader_;
		ShaderStage				pixelShader_;
		ShaderStage				computeShader_;

		Vector<String>			allDefs_;

		String					source_;
		uint					allMask_ = 0;

		HashMap<uint, SPtr<ShaderInstance>> cachedPass_;

		friend class Shader;
		friend class ShaderVariation;
		friend class ShaderInstance;
	};

	class Shader : public Resource
	{
		uRTTI(Shader, Resource)
	public:
		Shader();
		~Shader();

		virtual bool Prepare();
		
		virtual bool Create();

		Pass* AddPass(Pass* pass = nullptr);

		Pass* GetShaderPass(const StringID & pass);

		uint GetMask(const StringID& passName, const String & defs);

		ShaderInstance* GetInstance(const StringID& passName, uint defMask);

		ShaderInstance* GetInstance(const StringID& passName, const String & defs);

		static Vector<String> SplitDef(const String& defs);

		static String GetShaderPath(uint renderID);
	private:
		String shaderName_;
		Vector<SPtr<Pass>> passes_;
	};

}
