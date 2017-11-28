#pragma once
#include "../../resource/Resource.h"
#include "../GraphicsDefs.h"
#include "../GraphicsObject.h"

namespace Unique
{
	class Shader;
	class ShaderPass;

	class ShaderVariation : public RefCounted, public GraphicsObject<bgfx::ShaderHandle>
	{
	public:
		ShaderVariation(Shader& shader, ShaderType type, ShaderPass& shaderPass, unsigned defs);
		bool create();

		void reload();
		bool loadByteCode(const String & binaryShaderName);
		bool compile(const String& binaryShaderName);

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

	class ShaderInstance : public RefCounted, public GraphicsObject<bgfx::ProgramHandle>
	{
	public:
		ShaderInstance(Shader& shader, ShaderPass& shaderPass, unsigned defs);
		ShaderInstance(Shader& shader, ShaderPass& shaderPass, unsigned vsDefs, unsigned psDefs);

		bool Create();
		void reload();

		bgfx::ProgramHandle GetProgram()
		{
			if (dirty_ || !valid())
			{
				Create();
			}

			return handle_;

		}

		SPtr<ShaderVariation> vs;
		SPtr<ShaderVariation> ps;
		SPtr<ShaderVariation> cs;

		bool dirty_ = true;

	};
}
