#pragma once
#include "../../Resource/Resource.h"
#include "../GraphicsDefs.h"
#include "ShaderVariation.h"

namespace Unique
{

	struct ShaderProgramRecall
	{
		Vector<ShaderStage>		shaderDescs;
		Vector<LLGL::Shader*>	shaders;
		VertexFormat			vertexFormat;
		LLGL::StreamOutputFormat	streamOutputFormat;
	};

	class Shader;

	class SubShader : public Object
	{
		uRTTI(SubShader, Object)
	public:

		Vector<ShaderStage>& GetShaderStages() { return  shaderStages_; }

		uint GetMask(Shader* shader, const String& defs);

		ShaderInstance* GetInstance(Shader* shader, const String & defs);

		ShaderInstance* GetInstance(Shader* shader, unsigned defMask);
	private:
		StringID				name_;
		
		DepthState				depthState_;
		StencilState			stencilState_;
		RasterizerState			rasterizerState_;
		BlendState				blendState_;
		Vector<ShaderStage>		shaderStages_;
		Vector<String>			allDefs_;

		String					source_;
		uint					allMask_;

		HashMap<uint, SPtr<ShaderInstance>> cachedPass_;

		friend class Shader;
		friend class ShaderVariation;
	};

	class Shader : public Resource
	{
		uRTTI(Shader, Resource)
	public:
		Shader();
		~Shader();

		virtual bool BeginLoad(File& source);
		
		virtual bool EndLoad();

		const String& GetName() const { return name_; }
		void SetName(const String& name) { name_ = name;}

		SubShader* AddPass(SubShader* pass = nullptr);

		SubShader* GetShaderPass(const StringID & pass);

		uint GetMask(const StringID& passName, const String & defs);

		ShaderInstance* GetInstance(const StringID& passName, uint defMask);

		ShaderInstance* GetInstance(const StringID& passName, const String & defs);

		static Vector<String>&& SplitDef(const String& defs);

		static String GetShaderPath();
	private:
		String name_;
		String shaderDefines_;
		Vector<SPtr<SubShader>> passes_;
	};

		// Load standard shader program (with vertex- and fragment shaders)
	UNIQUE_C_API ShaderProgram* LoadStandardShaderProgram(const LLGL::VertexFormat& vertexFormat);

	UNIQUE_C_API ShaderProgram* LoadShaderProgram(
			const Vector<ShaderStage>& shaderDescs,
			const VertexFormat& vertexFormat = {},
			const LLGL::StreamOutputFormat& streamOutputFormat = {});

		// Reloads the specified shader program from the previously specified shader source files.
	UNIQUE_C_API bool ReloadShaderProgram(ShaderProgram* shaderProgram);

}
