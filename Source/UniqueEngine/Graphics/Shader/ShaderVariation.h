#pragma once
#include "resource/Resource.h"
#include "../GraphicsDefs.h"
#include "../GPUObject.h"
#include <Shader.h>
#include <PipelineState.h>

namespace Unique
{
	
	using ShaderType = SHADER_TYPE;

	class Shader;
	class Pass;

	struct ShaderStage
	{
		ShaderStage(ShaderType type = ShaderType::SHADER_TYPE_UNKNOWN) : shaderType_(type)
		{
		}

		ShaderStage(
			ShaderType type, const String& filename) :
			shaderType_{ type }
		{
		}

		ShaderStage(
			ShaderType type, const String& filename, const String& entryPoint, const String& target) :
			shaderType_{ type },
			entryPoint_{ entryPoint },
			target_{ target }
		{
		}

		operator bool()
		{
			return !entryPoint_.Empty();
		}

		uClass("EntryPoint", entryPoint_, "Target", target_, "Defines", defines_);

		ShaderType	shaderType_;
		String		entryPoint_;
		String		target_;
		String		defines_;
		uint		mask_;
	};

	class ShaderVariation : public GPUObject<RefCounted, IShader>
	{
	public:
		ShaderVariation(Shader& shader, const ShaderStage& type, Pass& shaderPass, uint defs);
		bool CreateImpl();

		void Reload();
		bool LoadByteCode(const String & binaryShaderName);
		bool Compile(const String& binaryShaderName);

	private:
		Shader& owner_;
		ShaderStage shaderStage_;
		Pass& shaderPass_;

		/// Defines to use in compiling.
		String defines_;
		unsigned mask_ = 0;
		bool dirty_ = false;
	};

	class ShaderProgram{};

	class ShaderInstance : public GPUObject<RefCounted, ShaderProgram>
	{
	public:
		ShaderInstance(Shader& shader, Pass& shaderPass, unsigned defs);

		bool CreateImpl();
		void Reload();

		IPipelineState* GetPipeline(/*const VertexFormat& vertexFormat*/);

		Vector<SPtr<ShaderVariation>>	shaders;
		bool dirty_ = true;
		IPipelineState*			pipeline_ = nullptr;

	};
}
