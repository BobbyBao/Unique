#pragma once
#include "../Resource/Resource.h"
#include "GraphicsDefs.h"

namespace Unique
{
	struct ShaderStage
	{
		ShaderStage() 
		{
		}

		ShaderStage(
			LLGL::ShaderType type, const String& filename) :
			type{ type },
			filename{ filename }
		{
		}

		ShaderStage(
			LLGL::ShaderType type, const String& filename, const String& entryPoint, const String& target) :
			type{ type },
			filename{ filename },
			entryPoint{ entryPoint },
			target{ target }
		{
		}

		uClass(ShaderStage,
			"Name", type,
			"FileName", filename,
			"EntryPoint", entryPoint,
			"Target", target);

		LLGL::ShaderType    type;
		String         filename;
		String         entryPoint;
		String         target;
	};

	struct ShaderProgramRecall
	{
		std::vector<ShaderStage>   shaderDescs;
		std::vector<LLGL::Shader*>              shaders;
		LLGL::VertexFormat                      vertexFormat;
		LLGL::StreamOutputFormat                streamOutputFormat;
	};

	class Pass : public Object
	{
		uRTTI(Pass, Object)
	public:

		Vector<ShaderStage>& GetShaderStages() { return  shaderStages_; }
		
	private:
		Vector<ShaderStage> shaderStages_;
		HashMap<uint64_t, class ShaderProgram*> shaderPrograms_;
	};


	class Technique : public Resource
	{
		uRTTI(Technique, Resource)
	public:
		Technique();
		~Technique();

		const String& GetName() const { return name_; }
		void SetName(const String& name) { name_ = name;}

		Pass* AddPass(Pass* pass = nullptr);

	private:
		String name_;
		String shaderDefines_;
		Vector<SPtr<Pass>> passes_;
	};

		// Load standard shader program (with vertex- and fragment shaders)
	UNIQUE_C_API LLGL::ShaderProgram* LoadStandardShaderProgram(const LLGL::VertexFormat& vertexFormat);

	UNIQUE_C_API LLGL::ShaderProgram* LoadShaderProgram(
			const Vector<ShaderStage>& shaderDescs,
			const LLGL::VertexFormat& vertexFormat = {},
			const LLGL::StreamOutputFormat& streamOutputFormat = {});

		// Reloads the specified shader program from the previously specified shader source files.
	UNIQUE_C_API bool ReloadShaderProgram(LLGL::ShaderProgram* shaderProgram);

}
