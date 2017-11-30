#pragma once
#include "../../Resource/Resource.h"
#include "../GraphicsDefs.h"
#include "ShaderVariation.h"

namespace Unique
{
	struct ShaderStage
	{
		ShaderStage() 
		{
		}

		ShaderStage(
			ShaderType type, const String& filename) :
			type{ type },
			filename{ filename }
		{
		}

		ShaderStage(
			ShaderType type, const String& filename, const String& entryPoint, const String& target) :
			type{ type },
			filename{ filename },
			entryPoint{ entryPoint },
			target{ target }
		{
		}

		uClass(	
			"Type", type,
			"FileName", filename,
			"EntryPoint", entryPoint,
			"Target", target);

		ShaderType	type;
		String		filename;
		String		entryPoint;
		String		target;
		uint		mask_;
	};

	struct ShaderProgramRecall
	{
		Vector<ShaderStage>		shaderDescs;
		Vector<LLGL::Shader*>	shaders;
		VertexFormat			vertexFormat;
		LLGL::StreamOutputFormat	streamOutputFormat;
	};

	class Shader;

	class ShaderPass : public Object
	{
		uRTTI(ShaderPass, Object)
	public:

		Vector<ShaderStage>& GetShaderStages() { return  shaderStages_; }

		uint GetMask(Shader* shader, const String& defs);

		ShaderInstance* GetInstance(Shader * shader, const String & defs);

		ShaderInstance* GetInstance(Shader * shader, unsigned defMask);
	private:
		StringID				name_;
		unsigned char			passIdx_;
		DepthState				depthState_;
		StencilState			stencilState_;
		RasterizerState			rasterizerState_;
		BlendState				blendState_;
		Vector<ShaderStage>		shaderStages_;
		Vector<String>			allDefs_;
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
		
		const String& GetName() const { return name_; }
		void SetName(const String& name) { name_ = name;}

		ShaderPass* AddPass(ShaderPass* pass = nullptr);

		ShaderPass* GetShaderPass(const StringID & pass);

		uint GetMask(const StringID& passName, const String & defs);

		ShaderInstance* GetInstance(const StringID& passName, uint defMask);

		ShaderInstance* GetInstance(const StringID& passName, const String & defs);

		static Vector<String>&& SplitDef(const String& defs);
		static String Shader::GetShaderPath();
	private:
		String name_;
		String shaderDefines_;
		Vector<SPtr<ShaderPass>> passes_;
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
