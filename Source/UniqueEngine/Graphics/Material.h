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

		const ResourceRef& GetShader() const { return shaderRes_; }
		void SetShader(const ResourceRef& shader);

		Shader* GetShader() { return shader_; }

	private:
		SPtr<Shader> shader_;
		String shaderDefines_;
		ResourceRef shaderRes_;
		Vector<ShaderVariable> shaderVaribles_;
	};


}
