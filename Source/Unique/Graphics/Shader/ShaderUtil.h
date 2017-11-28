#pragma once
#include "../../container/container.h"
#include "../GraphicsDefs.h"
#include "Shader.h"

namespace Unique
{
	class ShaderUtil
	{
	public:
		static void Init();
		static unsigned char GetPassIdx(const StringID& pass);
		static String GetShaderPath();
		static Vector<String> SplitDef(const String& defs);

	};

}

