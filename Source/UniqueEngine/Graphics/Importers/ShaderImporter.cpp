#include "Precompiled.h"
#include "ShaderImporter.h"
#include "../ShaderEffect.h"
#include "Serialize/DslDeserializer.h"

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
		DslDeserializer reader;
		SPtr<Shader> shader;
		if (!reader.Load(path, shader))
		{
			return nullptr;
		}

		return shader;
	}
}
