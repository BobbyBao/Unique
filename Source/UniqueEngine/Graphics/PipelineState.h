#pragma once
#include "GPUObject.h"

namespace Unique
{
	class ShaderVariation;
	class Shader;
	class Pass;
	
	class PipelineState : public RefCounted, public GPUObject
	{
	public:
		PipelineState(Shader& shader, Pass& shaderPass, unsigned defs);

		bool CreateImpl();
		void Reload();

		IShaderVariable* GetShaderVariable(const StringID& name) /*const*/;
		IPipelineState* GetPipeline();
		IShaderResourceBinding* GetShaderResourceBinding() { return shaderResourceBinding_; }
	private:
		Pass& shaderPass_;
		Vector<SPtr<ShaderVariation>> shaders_;
		HashMap<StringID, IShaderVariable*> shaderVariables_;
		RefCntAutoPtr<IShaderResourceBinding> shaderResourceBinding_;
		IResourceMapping* resourceMapping_;
		PipelineStateDesc psoDesc_;
		uint dirtyFlags_ = 0;
		bool isComputePipeline_ = false;
		bool dirty_ = true;
	};

}