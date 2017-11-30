#pragma once
#include "../../resource/Resource.h"
#include "../GraphicsDefs.h"
#include "../GfxObject.h"

namespace Unique
{
	class Shader;
	class ShaderPass;

	class ShaderVariation : public TGfxObject<RefCounted, LLGL::Shader>
	{
	public:
		ShaderVariation(Shader& shader, ShaderType type, ShaderPass& shaderPass, uint defs);
		bool CreateImpl();

		void Reload();
		bool LoadByteCode(const String & binaryShaderName);
		bool Compile(const String& binaryShaderName);

	private:
		String sourceFile() const;
		Shader& owner_;
		ShaderType type_;
		ShaderPass& shaderPass_;

		/// Defines to use in compiling.
		String defines_;
		unsigned mask_ = 0;
		bool dirty_ = false;
	};

	class ShaderInstance : public TGfxObject<RefCounted, ShaderProgram>
	{
	public:
		ShaderInstance(Shader& shader, ShaderPass& shaderPass, unsigned defs);

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
