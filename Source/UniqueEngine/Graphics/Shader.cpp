#include "Precompiled.h"
#include "Shader.h"
#include "Graphics/Graphics.h"
#include "PipelineState.h"


using namespace Diligent;

namespace Unique
{	

	uClassTraits
	(
		RenderTargetBlendDesc, 
		"BlendEnable", self.BlendEnable, 
		"LogicOperationEnable", self.LogicOperationEnable, 
		"SrcBlend", self.SrcBlend,
		"DestBlend", self.DestBlend, 
		"BlendOp", self.BlendOp,
		"SrcBlendAlpha", self.SrcBlendAlpha,
		"DestBlendAlpha", self.DestBlendAlpha,
		"BlendOpAlpha", self.BlendOpAlpha,
		"LogicOp", self.LogicOp,
		"RenderTargetWriteMask", (ColorMask&)self.RenderTargetWriteMask
	)
		
	uClassTraits
	(
		BlendState, 
		"AlphaToCoverageEnable", self.AlphaToCoverageEnable, 
		"IndependentBlendEnable", self.IndependentBlendEnable,
		"RenderTargets[0]", self.RenderTargets[0],
		"RenderTargets[1]", self.RenderTargets[1],
		"RenderTargets[2]", self.RenderTargets[2],
		"RenderTargets[3]", self.RenderTargets[3],
		"RenderTargets[4]", self.RenderTargets[4],
		"RenderTargets[5]", self.RenderTargets[5],
		"RenderTargets[6]", self.RenderTargets[6],
		"RenderTargets[7]", self.RenderTargets[7]
	)
	
	uEnum(LayoutElement::FREQUENCY, "UNDEFINED", "PER_VERTEX", "PER_INSTANCE")

	uClassTraits
	(
		LayoutElement, 
		"BufferSlot", self.BufferSlot,
		"NumComponents", self.NumComponents,
		"ValueType", self.ValueType,
		"IsNormalized", self.IsNormalized,
		"RelativeOffset", self.RelativeOffset,
		"Frequency", self.Frequency,
		"InstanceDataStepRate", self.InstanceDataStepRate
	)

	uObject(Pass)
	{
		uFactory("Graphics");
		uAttribute("Name", name_);
		uAttribute("Properties", shaderProperties_)
		uAttribute("DepthState", depthState_);
		uAttribute("RasterizerState", rasterizerState_);
		uAttribute("BlendState", blendState_);
		uAttribute("InputLayout", inputLayout_);
		uAttribute("VertexShader", shaderStage_[0]);
		uAttribute("PixelShader", shaderStage_[1]);
		uAttribute("GeometryShader", shaderStage_[2]);
		uAttribute("HullShader", shaderStage_[3]);
		uAttribute("DomainShader", shaderStage_[4]);
		uAttribute("ComputeShader", shaderStage_[5]);
	}

	uObject(Shader)
	{
		uFactory("Graphics")
		uAttribute("Name", shaderName_)
		uAttribute("Pass", passes_)	
	}


//	uExport(Pass, PipelineState*, GetPipeline, Shader*, shader, const String&, defs)


	Pass::Pass(const String& name) : name_(name)
	{
		for (int i = 0; i < 6; i++)
		{
			shaderStage_[i].shaderType_ = (ShaderType)(i + 1);
		}
	}

	Pass::~Pass()
	{
	}

	uint64 Pass::GetMask(Shader* shader, const String& interDefs, const String& defs)
	{
		uint64 mask = 0;
		for (uint i = 0; i < allDefs_.size(); i++)
		{
			if (defs.Find(allDefs_[i]) != String::NPOS)
			{
				mask |= (unsigned)(1 << i);
			}
		}

		uint64 interMask = 0;
		for (uint i = 0; i < ShaderUtil::interDefs.size(); i++)
		{
			if (interDefs.Find(ShaderUtil::interDefs[i]) != String::NPOS)
			{
				interMask |= (unsigned)(1 << i);
			}
		}

		return mask | (interMask << 32);
	}

	PipelineState* Pass::GetPipeline(Shader* shader, const String& interDefs, const String& defs)
	{
		uint64 defMask = GetMask(shader, interDefs, defs);

		return GetPipeline(shader, defMask);
	}

	PipelineState* Pass::GetPipeline(Shader* shader, uint64 defMask)
	{
		uint64 interMask = ShaderUtil::InterMask();
		defMask &= (allMask_ | interMask);

		auto it = cachedPipeline_.find(defMask);;
		if (it != cachedPipeline_.end())
		{
			return it->second;
		}

		SPtr<PipelineState> inst(new PipelineState(*shader, *this, defMask));
		cachedPipeline_[defMask] = inst;
		return inst;
		
	}

	bool Pass::Prepare()
	{
		passIndex_ = ShaderUtil::GetPassIndex(name_);

		for (size_t i = 0; i < inputLayout_.layoutElements_.size(); i++)
		{
			inputLayout_.layoutElements_[i].InputIndex = (uint)i;
		}

		auto& computeShader = shaderStage_[5];
		if (computeShader)
		{
			allDefs_ = ShaderUtil::SplitDef(computeShader.defines_);

			if (allDefs_.size() > 0)
			{
				std::sort(allDefs_.begin(), allDefs_.end());
				allMask_ = (unsigned)(1 << (allDefs_.size() + 1)) - 1;
			}
		}
		else
		{
			auto& vertexShader = shaderStage_[0];
			auto& pixelShader = shaderStage_[1];
			Vector<String> psDefs = ShaderUtil::SplitDef(pixelShader.defines_);
			Vector<String> vsDefs = ShaderUtil::SplitDef(vertexShader.defines_);
			allDefs_ = psDefs;

			for (auto& s : vsDefs)
			{
				if (!Contains(allDefs_, s))
				{
					allDefs_.push_back(s);
				}
			}
			
			if (allDefs_.size() > 0)
			{
				std::sort(allDefs_.begin(), allDefs_.end());
				allMask_ = (ShaderUtil::InterMask())|((1 << (allDefs_.size() + 1)) - 1);
			}

		}

		return true;
	}


	SPtr<ShaderVariation> Pass::GetShaderVariation(Shader& shader, const ShaderStage& shaderStage, uint64 defs)
	{
		auto& shaderCache = cachedShaders_[shaderStage.shaderType_ - 1];
		auto it = shaderCache.find(defs);
		if (it != shaderCache.end())
		{
			return it->second;
		}

		SPtr<ShaderVariation> shaderVariation(new ShaderVariation(shader, shaderStage, *this, defs));
		shaderCache[defs] = shaderVariation;
		return shaderVariation;
	}

	Shader::Shader()
	{
	}

	Shader::~Shader()
	{
	}

	bool Shader::Prepare()
	{
		for (auto& pass : passes_)
		{
			pass->Prepare();
		}

		return true;
	}

	bool Shader::Create()
	{
		return true;
	}

	Pass* Shader::AddPass(const String& name)
	{
		passes_.emplace_back(new Pass(name));
		return (passes_.back());
	}

	Pass* Shader::GetPass(const String& passName)
	{
		unsigned index = ShaderUtil::GetPassIndex(passName);
		return GetPass(index);
	}

	Pass* Shader::GetPass(unsigned passIndex)
	{
		for (auto& p : passes_)
		{
			if (p->passIndex_ == passIndex)
			{
				return p;
			}
		}

		return nullptr;
	}
		
	PipelineState* Shader::GetPipeline(const String& passName, const String& interDefs, const String& defs)
	{
		Pass* pass = GetPass(passName);
		if (pass == nullptr)
		{
			return nullptr;
		}

		return pass->GetPipeline(this, interDefs, defs);
	}
			
	PipelineState* Shader::GetPipeline(uint passIndex, const String& interDefs, const String& defs)
	{
		Pass* pass = GetPass(passIndex);
		if (pass == nullptr)
		{
			return nullptr;
		}

		return pass->GetPipeline(this, interDefs, defs);
	}

}