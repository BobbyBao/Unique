#pragma once
#include "GPUObject.h"

namespace Unique
{
	class ShaderVariation;
	class Shader;
	class Pass;

	class PipelineState : public RefCounted, public GPUObject<IPipelineState>
	{
	public:
		PipelineState(Shader& shader, Pass& shaderPass, unsigned defs);

		bool CreateImpl();
		void Reload();

		IPipelineState* GetPipeline();
		IShaderResourceBinding* GetShaderResourceBinding() { return shaderResourceBinding_; }
	private:
		Pass& shaderPass_;
		Vector<SPtr<ShaderVariation>> shaders;
		bool isComputePipeline_ = false;
		RefCntAutoPtr<IShaderResourceBinding> shaderResourceBinding_;
		bool dirty_ = true;
		IResourceMapping* resourceMapping_;
	};

}