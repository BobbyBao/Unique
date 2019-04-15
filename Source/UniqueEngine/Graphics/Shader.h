#pragma once
#include "Resource/Resource.h"
#include "ShaderVariation.h"
#include "ShaderVariable.h"
#include "RenderState.h"

namespace Unique
{
	class Shader;
	class PipelineState;

	/// Description of a single element of the input layout
	struct LayoutElement : public Diligent::LayoutElement
	{
		bool operator == ( const LayoutElement& rhs) const
		{
			return std::memcmp(this, &rhs, sizeof(*this)) == 0;
		}
	};

	struct InputLayout
	{
		Vector<LayoutElement> layoutElements_;
		
		bool operator ==( const InputLayout& rhs) const
		{
			return layoutElements_ == rhs.layoutElements_;
		}

		uClass("LayoutElement", layoutElements_)
	};
	
	class ShaderProperties
	{
	public:	
		Vector<Uniform>		uniforms_;
		Vector<TextureSlot> textureSlots_;
		
		bool operator == ( const ShaderProperties& rhs) const
		{
			return uniforms_ == rhs.uniforms_ && textureSlots_ == rhs.textureSlots_;
		}

		uClass("Uniform", uniforms_, "TextureSlot", textureSlots_)
	};

	class Pass : public Object
	{
		uRTTI(Pass, Object)
	public:
		Pass(const String& name = "");
		~Pass();

		uint64 GetMask(Shader* shader, const String& interDefs, const String& defs);
		PipelineState* GetPipeline(Shader* shader, const String& interDefs, const String & defs);
		PipelineState* GetPipeline(Shader* shader, uint64 defMask);
		bool Prepare();
		
		inline const auto& GetUniforms() const { return shaderProperties_.uniforms_; }
		inline const auto& GetTextureSlots() const { return shaderProperties_.textureSlots_; }
		const ShaderProperties& GetProperties() const { return shaderProperties_; }

		String					name_;
		uint					passIndex_ = 0;
		RasterizerState			rasterizerState_;
		DepthStencilState		depthState_;
		BlendState				blendState_;
		InputLayout				inputLayout_;
		ShaderStage				shaderStage_[6];

	private:
		SPtr<ShaderVariation>	GetShaderVariation(Shader& shader, const ShaderStage& shaderStage, uint64 defs);
	
		ShaderProperties		shaderProperties_;

		Vector<String>			allDefs_;
		uint64					allMask_ = 0;

		HashMap<uint64, SPtr<PipelineState>> cachedPipeline_;
		HashMap<uint64, SPtr<ShaderVariation>> cachedShaders_[6];

		friend class Shader;
		friend class ShaderVariation;
		friend class PipelineState;
	};

	class Shader : public Resource
	{
		uRTTI(Shader, Resource)
	public:
		Shader();
		~Shader();

		virtual bool Prepare();
		virtual bool Create();

		Pass* AddPass(const String& name);
		Pass* GetPass(const String & pass);
		Pass* GetPass(unsigned passIndex);

		PipelineState* GetPipeline(const String& passName, const String& interDefs, const String & defs);
		PipelineState * GetPipeline(uint passIndex, const String & interDefs, const String & defs);

	private:
		String				shaderName_;
		Vector<SPtr<Pass>>	passes_;

	};

}
