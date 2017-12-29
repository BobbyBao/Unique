#pragma once
#include "GPUObject.h"

namespace Unique
{
	class ShaderVariation;
	class Shader;
	class Pass;

	class ShaderProgram : RefCounted
	{
	public:
	};

	class PipelineState : public RefCounted, public GPUObject
	{
	public:
		PipelineState(Shader& shader, Pass& shaderPass, unsigned defs);

		bool CreateImpl();
		void Reload();

		IShaderVariable* GetShaderVariable(const StringID& name) const;
		IPipelineState* GetPipeline();
		IShaderResourceBinding* GetShaderResourceBinding() { return shaderResourceBinding_; }
	private:
		Vector<SPtr<ShaderVariation>> shaders_;
		HashMap<StringID, IShaderVariable*> shaderVariables_;
		Pass& shaderPass_;
		bool isComputePipeline_ = false;
		RefCntAutoPtr<IShaderResourceBinding> shaderResourceBinding_;
		bool dirty_ = true;
		uint dirtyFlags_ = 0;
		IResourceMapping* resourceMapping_;
		PipelineStateDesc PSODesc;
	};

}