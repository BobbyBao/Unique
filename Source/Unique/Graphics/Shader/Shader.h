#pragma once
#include "../../Resource/Resource.h"
#include "../GraphicsDefs.h"

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
			"Name", type,
			"FileName", filename,
			"EntryPoint", entryPoint,
			"Target", target);

		ShaderType	type;
		String		filename;
		String		entryPoint;
		String		target;
		uint64_t	mask_;
	};

	struct ShaderProgramRecall
	{
		Vector<ShaderStage>		shaderDescs;
		Vector<LLGL::Shader*>	shaders;
		VertexFormat			vertexFormat;
		LLGL::StreamOutputFormat	streamOutputFormat;
	};

	class ShaderPass : public Object
	{
		uRTTI(ShaderPass, Object)
	public:

		Vector<ShaderStage>& GetShaderStages() { return  shaderStages_; }
		
	private:
		StringID		name_;
		Vector<ShaderStage> shaderStages_;
		GraphicsPipeline* pipeline_;
		HashMap<uint64_t, ShaderProgram*> shaderPrograms_;
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

		ShaderProgram* GetShaderProgram(const StringID& pass, uint64_t defines);

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
