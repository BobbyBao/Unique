#pragma once
#include "../../resource/Resource.h"
#include "../GraphicsDefs.h"
#include "../GPUObject.h"

namespace Unique
{

	class Shader;
	class Pass;

	struct ShaderStage
	{
		ShaderStage(ShaderType type = ShaderType::Vertex) : shaderType_(type)
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

	class ShaderVariation : public GPUObject<RefCounted, LLGL::Shader>
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

	class ShaderInstance : public GPUObject<RefCounted, ShaderProgram>
	{
	public:
		ShaderInstance(Shader& shader, Pass& shaderPass, unsigned defs);

		bool CreateImpl();
		void Reload();

		GraphicsPipeline* GetPipeline(const VertexFormat& vertexFormat);

		Vector<SPtr<ShaderVariation>>	shaders;
		bool dirty_ = true;
		GraphicsPipeline*			pipeline_ = nullptr;

	};
}
