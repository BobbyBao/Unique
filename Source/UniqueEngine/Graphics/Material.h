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

		void SetTexture(const StringID& name, Texture* texture);
		void Apply();
	protected:
		virtual void CreateImpl();
	private:
		SPtr<Shader> shader_;
		String shaderDefines_;
		ResourceRef shaderRes_;
		Vector<Uniform> uniforms_;
		Vector<TextureSlot> textureSlots_;
	};


}
