#pragma once
#include "GPUObject.h"
#include "ShaderEffect.h"

namespace Unique
{
	using DepthStencilStateDesc = Diligent::DepthStencilStateDesc;
	using IShaderVariable = Diligent::IShaderVariable;

	class ShaderVariation;
	class Shader;
	class Pass;

	class ShaderProgram : public RefCounted
	{
	public:
		ShaderProgram(Pass& shaderPass) : shaderPass_(shaderPass){}
		Pass& shaderPass_;
		Vector<SPtr<ShaderVariation>> shaders_;
		bool isComputePipeline_ = false;
	};
	
	class PipelineState : public RefCounted, public GPUObject
	{
	public:
		PipelineState(Shader& shader, Pass& shaderPass, unsigned defs);
		PipelineState(ShaderProgram* shaderProgram);

		bool CreateImpl();
		void Reload();

		ShaderProgram* GetShaderProgram() { return shaderProgram_; }
		IShaderVariable* GetShaderVariable(const StringID& name);
		IPipelineState* GetPipeline();
		IShaderResourceBinding* GetShaderResourceBinding() { return shaderResourceBinding_; }

		void SetDepthStencilState(const DepthStencilStateDesc& dss);
		void SetCullMode(CullMode cull);
		void SetLineAntialiased(bool val);

	private:
		void Init();
		SPtr<ShaderProgram> shaderProgram_;
		Diligent::RefCntAutoPtr<IShaderResourceBinding> shaderResourceBinding_;
		IResourceMapping* resourceMapping_;
		Diligent::PipelineStateDesc psoDesc_;
		bool shaderDirty_ = true;
		bool dirty_ = true;
	};

}