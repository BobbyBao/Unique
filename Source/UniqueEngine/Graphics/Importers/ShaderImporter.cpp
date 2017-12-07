#include "Precompiled.h"
#include "ShaderImporter.h"
#include "../Shader.h"


namespace Unique
{
	ShaderImporter::ShaderImporter() : ResourceImporter(Shader::GetTypeStatic())
	{
	}

	ShaderImporter::~ShaderImporter()
	{
	}

	SPtr<Resource> ShaderImporter::Import(const String& path)
	{
		JsonReader reader;
		SPtr<Shader> shader;
		if (!reader.Load(path, shader, true, true))
		{
			return nullptr;
		}

		return shader;
	}
}
