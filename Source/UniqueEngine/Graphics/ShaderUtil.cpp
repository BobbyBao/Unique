#include "UniquePCH.h"
#include "ShaderUtil.h"

namespace Unique
{

	unsigned ShaderUtil::basePassIndex = 0;
	unsigned ShaderUtil::alphaPassIndex = 0;
	unsigned ShaderUtil::materialPassIndex = 0;
	unsigned ShaderUtil::deferredPassIndex = 0;
	unsigned ShaderUtil::lightPassIndex = 0;
	unsigned ShaderUtil::litBasePassIndex = 0;
	unsigned ShaderUtil::litAlphaPassIndex = 0;
	unsigned ShaderUtil::shadowPassIndex = 0;

	HashMap<String, unsigned> ShaderUtil::passIndices;

	unsigned ShaderUtil::GetPassIndex(const String& passName)
	{
		// Initialize built-in pass indices on first call
		if (passIndices.empty())
		{
			basePassIndex = passIndices["base"] = 0;
			alphaPassIndex = passIndices["alpha"] = 1;
			materialPassIndex = passIndices["material"] = 2;
			deferredPassIndex = passIndices["deferred"] = 3;
			lightPassIndex = passIndices["light"] = 4;
			litBasePassIndex = passIndices["litbase"] = 5;
			litAlphaPassIndex = passIndices["litalpha"] = 6;
			shadowPassIndex = passIndices["shadow"] = 7;
		}

		if (passName.Empty())
		{
			return 0;
		}

		String nameLower = passName.ToLower();
		auto i = passIndices.find(nameLower);
		if (i != passIndices.end())
			return i->second;
		else
		{
			unsigned newPassIndex = (uint)passIndices.size();
			passIndices[nameLower] = newPassIndex;
			return newPassIndex;
		}
	}

	String ShaderUtil::GetShaderPath(DeviceType deviceType)
	{
		switch (deviceType)
		{
		case DeviceType::D3D11:
		case DeviceType::D3D12:
			return "Shaders/HLSL/";

		case DeviceType::OpenGL:
		case DeviceType::OpenGLES:
			return "Shaders/GLSL/";
		default:
			return "";
		}
	}

	Vector<String> ShaderUtil::SplitDef(const String& defs)
	{
		if (defs.Empty())
		{
			return Vector<String>();
		}

		return defs.Split(' ');
	}

	const Vector<String> ShaderUtil::interDefs =
	{
		"SKINNED",
		"INSTANCED",
		"BILLBOARD",
		"DIRBILLBOARD",
		"TRAILFACECAM",
		"TRAILBONE",
		"",
	};

}