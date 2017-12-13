#pragma once
#include "Resource/Resource.h"
#include "Shader.h"

namespace Diligent
{
	class IResourceMapping;
}

namespace Unique
{
	class Material : public Resource
	{
		uRTTI(Material, Resource)
	public:
		Material();
		~Material();

	private:
		SPtr<Shader> shader_;
		Vector<ShaderVariable> shaderVaribles_;
	};


}
