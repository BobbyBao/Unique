#pragma once
#include "Resource/Resource.h"
#include "Shader.h"

namespace Unique
{
	class Material : public Resource
	{
		uRTTI(Material, Resource)
	public:
		Material();
		~Material();

		const ResourceRef& GetShaderAttr() const { return shaderAttr_; }
		void SetShaderAttr(const ResourceRef& shader);

		Shader* GetShader() { return shader_; }
		void SetShader(Shader* shader) { shader_ = shader; }

		void SetTexture(const StringID& name, Texture* texture);
		void Apply(PipelineState* pipeline);
	protected:
		virtual bool Prepare();
		virtual bool Create();
	private:
		SPtr<Shader> shader_;
		String shaderDefines_;
		ResourceRef shaderAttr_;
		Vector<Uniform> uniforms_;
		Vector<TextureSlot> textureSlots_;
	};


}
