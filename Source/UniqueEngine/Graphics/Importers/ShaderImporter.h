#pragma once
#include "Resource/ResourceImporter.h"

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

