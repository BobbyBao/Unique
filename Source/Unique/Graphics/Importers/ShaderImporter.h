#pragma once
#include "../../resource/ResourceImporter.h"

namespace Unique
{

	class ShaderImporter : public ResourceImporter
	{
	public:
		ShaderImporter();
		~ShaderImporter();

		virtual SPtr<Resource> Import(const String& path);
	private:

	};

}

