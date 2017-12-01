#pragma once
#include "../../resource/Resource.h"
#include "../GraphicsDefs.h"
#include "../GfxObject.h"

namespace Unique
{

	class Shader;
	class SubShader;

	struct ShaderStage
	{
		ShaderStage()
		{
		}

		ShaderStage(
			ShaderType type, const String& filename) :
			name_{ type }
		{
		}

		ShaderStage(
			ShaderType type, const String& filename, const String& entryPoint, const String& target) :
			name_{ type },
			entryPoint_{ entryPoint },
			target_{ target }
		{
		}

		uClass(
			"Name", name_,
			"EntryPoint", entryPoint_,
			"Target", target_);

		ShaderType	name_;
		String		entryPoint_;
		String		target_;
		uint		mask_;
	};

	class ShaderVariation : public TGfxObject<RefCounted, LLGL::Shader>
	{
	public:
		ShaderVariation(Shader& shader, const ShaderStage& type, SubShader& shaderPass, uint defs);
		bool CreateImpl();

		void Reload();
		bool LoadByteCode(const String & binaryShaderName);
		bool Compile(const String& binaryShaderName);

	private:
		Shader& owner_;
		ShaderStage shaderStage_;
		SubShader& shaderPass_;

		/// Defines to use in compiling.
		String defines_;
		unsigned mask_ = 0;
		bool dirty_ = false;
	};

	class ShaderInstance : public TGfxObject<RefCounted, ShaderProgram>
	{
	public:
		ShaderInstance(Shader& shader, SubShader& shaderPass, unsigned defs);

		bool CreateImpl();
		void Reload();

		ShaderProgram* GetProgram()
		{
			if (dirty_ || !IsValid())
			{
				CreateImpl();
			}

			return (ShaderProgram*)handle_;
		}

		Vector<SPtr<ShaderVariation>>	shaders;
		bool dirty_ = true;
		GraphicsPipeline*			pipeline_;

	};
}
