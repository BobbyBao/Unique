#pragma once
#include "Container/StringID.h"
#include "../GraphicsDefs.h"
#include <LLGL/ShaderUniform.h>


namespace Unique
{
	class Uniform
	{
	public:
		Uniform();
		~Uniform();

		StringID		name_;							//!< Name of the uniform inside the shader.
		UniformType		type_ = UniformType::Float;	//!< Data type of the uniform.
		ushort			location_ = 0;					//!< Internal location of the uniform within a shader program.
		uint			size_ = 0;						//!< Array size of the uniform.
	
		uClass(
			"Name", name_,
			"Type", type_);
	};

	class UniformGroup
	{
	public:

		Vector<Uniform> uniforms;

		uClass("Uniforms", uniforms);
	};

}

