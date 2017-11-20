#include "Precompiled.h"
#include "Shader.h"

namespace Unique
{
	uObject(Shader)
	{
		uFactory("Graphics")
		uAccessor("Name", GetName, SetName, String, TF_DEFAULT)
		uAttribute("ShaderDefines", shaderDefines_, String, TF_DEFAULT)
		uAttribute("ShaderStages", shaderStages_, Vector<ShaderStage>, TF_DEFAULT)
	}

	Shader::Shader()
	{

	}


	Shader::~Shader()
	{
	}

}