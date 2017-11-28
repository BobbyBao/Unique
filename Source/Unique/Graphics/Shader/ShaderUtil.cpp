#include "Precompiled.h"
#include "ShaderUtil.h"

using namespace Unique;


Vector<StringID> passIDs_;

void ShaderUtil::Init()
{
	passIDs_.push_back("main");

}

String ShaderUtil::GetShaderPath()
{
	if (Subsystem<Graphics>().IsOpenGL())
	{
		return "ShaderCache/glsl/";
	}

	return "";
	
}


unsigned char ShaderUtil::GetPassIdx(const StringID& pass)
{
	if (pass.empty())
	{
		return 0;
	}

	for (unsigned char i = 0; i < passIDs_.size(); i++)
	{
		if (passIDs_[i] == pass)
		{
			return i;
		}
	}

	passIDs_.push_back(pass);
	return (unsigned char)(passIDs_.size() - 1);
	
}

Vector<String> ShaderUtil::SplitDef(const String& defs)
{
	if (defs.Empty())
	{
		return Vector<String>();
	}

	return defs.Split(' ');
}