#include "Precompiled.h"
#include "ShaderImporter.h"
#include "../Shader.h"
#include "Serialize/HjsonDeserializer.h"

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
		HjsonDeserializer reader(true);
		SPtr<Shader> shader;
		if (!reader.Load(path, shader))
		{
			return nullptr;
		}

		return shader;
	}
}
