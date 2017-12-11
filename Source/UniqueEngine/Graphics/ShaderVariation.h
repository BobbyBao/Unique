#pragma once
#include "resource/Resource.h"
#include "GraphicsDefs.h"
#include "GPUObject.h"
#include <RefCntAutoPtr.h>
#include <Shader.h>
#include <PipelineState.h>
#include <ShaderResourceBinding.h>

namespace Unique
{
	using ShaderType = SHADER_TYPE;
	using ShaderProfile = SHADER_PROFILE;

	class Shader;
	class Pass;

	uEnumTraits(ShaderProfile, "DEFAULT", "DX_4_0", "DX_5_0", "DX_5_1", "GL_4_2")

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
			ShaderType type, const String& filename, const String& entryPoint, const ShaderProfile& target) :
			shaderType_{ type },
			entryPoint_{ entryPoint },
			shaderProfile_{ target }
		{
		}

		operator bool()
		{
			return !source_.Empty();
		}

		uClass("EntryPoint", entryPoint_, "ShaderProfile", shaderProfile_, "Defines", defines_, "Source", source_);

		ShaderType		shaderType_;
		String			entryPoint_;
		ShaderProfile	shaderProfile_;
		String			defines_;
		String			source_;
		uint			mask_;
	};

	class ShaderVariation : public RefCounted, public GPUObject<IShader>
	{
	public:
		ShaderVariation(Shader& shader, const ShaderStage& type, Pass& shaderPass, uint defs);
		bool CreateImpl();

		void Reload();
		bool LoadConvertedCode(const String & binaryShaderName);
		bool Convert(const String& binaryShaderName);
		ShaderType GetShaderType() const { return shaderStage_.shaderType_; }
	private:
		Shader& owner_;
		ShaderStage shaderStage_;
		Pass& shaderPass_;

		/// Defines to use in compiling.
		String defines_;
		unsigned mask_ = 0;
		bool dirty_ = false;
	};
	
}
