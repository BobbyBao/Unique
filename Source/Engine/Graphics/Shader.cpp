#include "../Precompiled.h"
#include "Shader.h"

namespace Unique
{
	uObject(Shader)
	{
		uFactory("Graphics")
		uAccessor("Name", GetName, SetName, String, 0)
		uAttribute("ShaderDefines", shaderDefines_, String, 0)
	}

	Shader::Shader()
	{

	}


	Shader::~Shader()
	{
	}

}