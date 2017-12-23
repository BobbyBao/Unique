#include "Precompiled.h"
#include "Shader.h"
#include "Graphics/Graphics.h"
#include "PipelineState.h"

namespace Unique
{	
	uClassTraits
	(
		StencilOpDesc, 
		"StencilFailOp", self.StencilFailOp, 
		"StencilDepthFailOp", self.StencilDepthFailOp, 
		"StencilPassOp", self.StencilPassOp,
		"StencilFunc", self.StencilFunc
	)
	
	uClassTraits
	(
		DepthStencilStateDesc, 
		"DepthEnable", self.DepthEnable, 
		"DepthWriteEnable", self.DepthWriteEnable, 
		"ComparisonFunction", self.DepthFunc,
		"StencilEnable", self.StencilEnable, 
		"StencilReadMask", self.StencilReadMask,
		"StencilWriteMask", self.StencilWriteMask,
		"FrontFace", self.FrontFace,
		"BackFace", self.BackFace
	)

	uClassTraits
	(
		RasterizerStateDesc, 
		"FillMode", self.FillMode, 
		"CullMode", self.CullMode, 
		"FrontCounterClockwise", self.FrontCounterClockwise,
		"DepthBias", self.DepthBias, 
		"DepthBiasClamp", self.DepthBiasClamp,
		"SlopeScaledDepthBias", self.SlopeScaledDepthBias,
		"DepthClipEnable", self.DepthClipEnable,
		"ScissorEnable", self.ScissorEnable,
		"AntialiasedLineEnable", self.AntialiasedLineEnable
	)

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
		BlendStateDesc, 
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
	
	uEnumTraits(LayoutElement::FREQUENCY, "UNDEFINED", "PER_VERTEX", "PER_INSTANCE")
		
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

	uClassTraits
	(
		Pass,
		"Name", self.name_,
		"DepthState", self.depthState_,
		"RasterizerState", self.rasterizerState_,
		"BlendStateDesc", self.blendState_,
		"InputLayout", self.inputLayout_,
		"VertexShader", self.shaderStage_[0],
		"PixelShader", self.shaderStage_[1],
		"GeometryShader", self.shaderStage_[2],
		"HullShader", self.shaderStage_[3],
		"DomainShader", self.shaderStage_[4],
		"ComputeShader", self.shaderStage_[5]
	)

	uObject(Shader)
	{
		uFactory("Graphics")
		uAttribute("Name", shaderName_)
		uAttribute("Pass", passes_)	
	}

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

	uint Pass::GetMask(Shader* shader, const String& defs)
	{
		unsigned mask = 0;
		for (uint i = 0; i < allDefs_.size(); i++)
		{
			if (defs.Find(allDefs_[i]) != String::NPOS)
			{
				mask |= (unsigned)(1 << i);
			}
		}

		return mask;
	}

	PipelineState* Pass::GetPipeline(Shader* shader, const String& defs)
	{
		unsigned defMask = GetMask(shader, defs);

		return GetPipeline(shader, defMask);
	}

	PipelineState* Pass::GetPipeline(Shader* shader, unsigned defMask)
	{
		defMask &= allMask_;

		auto it = cachedPass_.find(defMask);;
		if (it != cachedPass_.end())
		{
			return it->second;
		}

		SPtr<PipelineState> inst(new PipelineState(*shader, *this, defMask));
		cachedPass_[defMask] = inst;
		return inst;
		
	}

	bool Pass::Prepare()
	{
		passIndex_ = Shader::GetPassIndex(name_);

		for (size_t i = 0; i < inputLayout_.layoutElements_.size(); i++)
		{
			inputLayout_.layoutElements_[i].InputIndex = (uint)i;
		}

		auto& computeShader = shaderStage_[5];
		if (computeShader)
		{
			allDefs_ = Shader::SplitDef(computeShader.defines_);

			if (allDefs_.size() > 0)
			{
				std::sort(allDefs_.begin(), allDefs_.end());
				allMask_ = (unsigned)(1 << (allDefs_.size() + 1)) - 1;
				computeShader.mask_ = allMask_;
			}
		}
		else
		{
			auto& vertexShader = shaderStage_[0];
			auto& pixelShader = shaderStage_[1];
			Vector<String> psDefs = Shader::SplitDef(pixelShader.defines_);
			Vector<String> vsDefs = Shader::SplitDef(vertexShader.defines_);
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
				allMask_ = (unsigned)(1 << (allDefs_.size() + 1)) - 1;

				for (uint i = 0; i < allDefs_.size(); i++)
				{
					if (Find(vsDefs, allDefs_[i]) != vsDefs.end())
					{
						pixelShader.mask_ |= (unsigned)(1 << i);
					}

					if (Find(psDefs, allDefs_[i]) != psDefs.end())
					{
						pixelShader.mask_ |= (unsigned)(1 << i);
					}
				}
			}

		}

		return true;
	}

	unsigned Shader::basePassIndex = 0;
	unsigned Shader::alphaPassIndex = 0;
	unsigned Shader::materialPassIndex = 0;
	unsigned Shader::deferredPassIndex = 0;
	unsigned Shader::lightPassIndex = 0;
	unsigned Shader::litBasePassIndex = 0;
	unsigned Shader::litAlphaPassIndex = 0;
	unsigned Shader::shadowPassIndex = 0;
	HashMap<String, unsigned> Shader::passIndices;

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
			pass.Prepare();
		}

		return true;
	}

	bool Shader::Create()
	{
		return true;
	}

	Pass* Shader::AddPass(const String& name)
	{
		passes_.emplace_back(name);
		return &(passes_.back());
	}

	Pass* Shader::GetShaderPass(const String& passName)
	{
		unsigned index = GetPassIndex(passName);
		return GetShaderPass(index);
	}

	Pass* Shader::GetShaderPass(unsigned passIndex)
	{
		for (auto& p : passes_)
		{
			if (p.passIndex_ == passIndex)
			{
				return &p;
			}
		}

		return nullptr;
	}

	uint Shader::GetMask(const String & passName, const String& defs)
	{
		Pass* pass = GetShaderPass(passName);
		if (pass == nullptr)
		{
			return 0;
		}

		return pass->GetMask(this, defs);
	}

	PipelineState* Shader::GetPipeline(const String& passName, uint defMask)
	{
		Pass* pass = GetShaderPass(passName);
		if (pass == nullptr)
		{
			return nullptr;
		}

		return pass->GetPipeline(this, defMask);
	}

	PipelineState* Shader::GetPipeline(const String& passName, const String& defs)
	{
		Pass* pass = GetShaderPass(passName);
		if (pass == nullptr)
		{
			return nullptr;
		}

		return pass->GetPipeline(this, defs);
	}

	unsigned Shader::GetPassIndex(const String& passName)
	{
		// Initialize built-in pass indices on first call
		if (passIndices.empty())
		{
			basePassIndex = passIndices["base"] = 0;
			alphaPassIndex = passIndices["alpha"] = 1;
			materialPassIndex = passIndices["material"] = 2;
			deferredPassIndex = passIndices["deferred"] = 3;
			lightPassIndex = passIndices["light"] = 4;
			litBasePassIndex = passIndices["litbase"] = 5;
			litAlphaPassIndex = passIndices["litalpha"] = 6;
			shadowPassIndex = passIndices["shadow"] = 7;
		}

		if (passName.Empty())
		{
			return 0;
		}

		String nameLower = passName.ToLower();
		auto i = passIndices.find(nameLower);
		if (i != passIndices.end())
			return i->second;
		else
		{
			unsigned newPassIndex = (uint)passIndices.size();
			passIndices[nameLower] = newPassIndex;
			return newPassIndex;
		}
	}

	Vector<String> Shader::SplitDef(const String& defs)
	{
		if (defs.Empty())
		{
			return Vector<String>();
		}

		return defs.Split(' ');
	}
	


}