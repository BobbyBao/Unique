#pragma once
#include "../Core/Object.h"
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


	class Shader : public Object
	{
		uRTTI(Shader, Object)
	public:
		Shader();
		~Shader();

		const String& GetName() const { return name_; }
		void SetName(const String& name) { name_ = name;}

		Vector<ShaderStage>& GetShaderStages() { return  shaderStages_; }

	private:
		String name_;
		String shaderDefines_;
		Vector<ShaderStage> shaderStages_;
	};

		// Load standard shader program (with vertex- and fragment shaders)
	UNIQUE_C_API LLGL::ShaderProgram* LoadStandardShaderProgram(const LLGL::VertexFormat& vertexFormat);

	UNIQUE_C_API LLGL::ShaderProgram* LoadShaderProgram(
			const std::vector<ShaderStage>& shaderDescs,
			const LLGL::VertexFormat& vertexFormat = {},
			const LLGL::StreamOutputFormat& streamOutputFormat = {});

		// Reloads the specified shader program from the previously specified shader source files.
	UNIQUE_C_API bool ReloadShaderProgram(LLGL::ShaderProgram* shaderProgram);

}
