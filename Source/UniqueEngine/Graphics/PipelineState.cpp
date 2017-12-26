#include "UniquePCH.h"
#include "PipelineState.h"
#include "Shader.h"

namespace Unique
{

	PipelineState::PipelineState(Shader& shader, Pass& shaderPass, unsigned defs) : shaderPass_(shaderPass)
	{
		if (shaderPass.shaderStage_[5])
		{
			isComputePipeline_ = true;
		}
		else
		{
			isComputePipeline_ = false;

			for (int i = 0; i < 5; i++)
			{
				if (shaderPass.shaderStage_[i])
				{
					SPtr<ShaderVariation> sv(new ShaderVariation(shader, shaderPass.shaderStage_[i], shaderPass, defs));
					shaders.push_back(sv);
				}
			}

		}

	}

	bool PipelineState::CreateImpl()
	{
		for (auto& shader : shaders)
		{
			if (!shader->CreateImpl())
			{
				return false;
			}

		}

		PipelineStateDesc PSODesc;
		PSODesc.IsComputePipeline = isComputePipeline_;
		PSODesc.GraphicsPipeline.NumRenderTargets = 1;
		PSODesc.GraphicsPipeline.RTVFormats[0] = TEX_FORMAT_RGBA8_UNORM_SRGB;
		PSODesc.GraphicsPipeline.DSVFormat = TEX_FORMAT_D32_FLOAT;

		PSODesc.GraphicsPipeline.DepthStencilDesc = shaderPass_.depthState_;
		PSODesc.GraphicsPipeline.RasterizerDesc = shaderPass_.rasterizerState_;
		PSODesc.GraphicsPipeline.BlendDesc = shaderPass_.blendState_;

		PSODesc.GraphicsPipeline.InputLayout.LayoutElements = shaderPass_.inputLayout_.layoutElements_.data();
		PSODesc.GraphicsPipeline.InputLayout.NumElements = (uint)shaderPass_.inputLayout_.layoutElements_.size();

		PSODesc.GraphicsPipeline.PrimitiveTopologyType = PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		if (isComputePipeline_)
		{
			for (auto shader : shaders)
			{
				if (shader->GetShaderType() == SHADER_TYPE_COMPUTE)
				{
					PSODesc.ComputePipeline.pCS = *shader;
					for (auto it : shader->shaderVariables_)
					{
						shaderVariables_.insert(it);
					}
					break;
				}
			}
		}
		else
		{
			for (auto shader : shaders)
			{
				for (auto it : shader->shaderVariables_)
				{
					shaderVariables_.insert(it);
				}

				switch (shader->GetShaderType())
				{
				case SHADER_TYPE_VERTEX:
					PSODesc.GraphicsPipeline.pVS = *shader;
					break;
				case SHADER_TYPE_PIXEL:
					PSODesc.GraphicsPipeline.pPS = *shader;
					break;
				case SHADER_TYPE_GEOMETRY:
					PSODesc.GraphicsPipeline.pGS = *shader;
					break;
				case SHADER_TYPE_HULL:
					PSODesc.GraphicsPipeline.pHS = *shader;
					break;
				case SHADER_TYPE_DOMAIN:
					PSODesc.GraphicsPipeline.pDS = *shader;
					break;
				default:
					break;
				}

			}
		}

		auto& graphics = GetSubsystem<Graphics>();
		renderDevice->CreatePipelineState(PSODesc, (IPipelineState**)&deviceObject_);
		graphics.BindShaderResources(*this, BIND_SHADER_RESOURCES_ALL_RESOLVED);
		((IPipelineState*)deviceObject_)->CreateShaderResourceBinding(&shaderResourceBinding_);
		dirty_ = false;
		return true;
	}

	IPipelineState* PipelineState::GetPipeline()
	{
		if (dirty_ || !IsValid())
		{
			if (!CreateImpl())
			{
				return nullptr;
			}
		}

		return (IPipelineState*)deviceObject_;
	}

	void PipelineState::Reload()
	{
		dirty_ = true;

		for (auto& shd : shaders)
		{
			shd->Reload();
		}

	}


}