#include "UniquePCH.h"
#include "RenderPass.h"

namespace Unique
{		
	RenderPass::RenderPass(RenderPassType type) :
		type_(type),	
		enabled_(true),
		clearFlags_(0),
		clearColor_(Color::GRAY),
		clearDepth_(1.0f),
		clearStencil_(0xff),
		//blendMode_(BLEND_REPLACE),
		useFogColor_(false),
		markToStencil_(false),
		useLitBase_(true),
		vertexLights_(false),
		graphics_(GetSubsystem<Graphics>())
	{
	}


	void RenderPass::SetTextureName(TextureUnit unit, const String& name)
	{
		if (unit < MAX_TEXTURE_UNITS)
			textureNames_[unit] = name;
	}

	void RenderPass::SetNumOutputs(unsigned num)
	{
		num = (unsigned)Clamp((int)num, 1, MAX_RENDERTARGETS);
		outputs_.resize(num);
	}

	void RenderPass::SetOutput(unsigned index, const String& name, CubeMapFace face)
	{
		if (index < outputs_.size())
			outputs_[index] = MakePair(name, face);
		else if (index == outputs_.size() && index < MAX_RENDERTARGETS)
			outputs_.push_back(MakePair(name, face));
	}

	void RenderPass::SetOutputName(unsigned index, const String& name)
	{
		if (index < outputs_.size())
			outputs_[index].first = name;
		else if (index == outputs_.size() && index < MAX_RENDERTARGETS)
			outputs_.push_back(MakePair(name, FACE_POSITIVE_X));
	}

	void RenderPass::SetOutputFace(unsigned index, CubeMapFace face)
	{
		if (index < outputs_.size())
			outputs_[index].second = face;
		else if (index == outputs_.size() && index < MAX_RENDERTARGETS)
			outputs_.push_back(MakePair(String::EMPTY, face));
	}


	void RenderPass::SetDepthStencilName(const String& name)
	{
		depthStencilName_ = name;
	}

	const String& RenderPass::GetTextureName(TextureUnit unit) const
	{
		return unit < MAX_TEXTURE_UNITS ? textureNames_[unit] : String::EMPTY;
	}

	const String& RenderPass::GetOutputName(unsigned index) const
	{
		return index < outputs_.size() ? outputs_[index].first : String::EMPTY;
	}

	CubeMapFace RenderPass::GetOutputFace(unsigned index) const
	{
		return index < outputs_.size() ? outputs_[index].second : FACE_POSITIVE_X;
	}
	
	void RenderPass::Update()
	{
	}
	
	void RenderPass::Render(View* view)
	{
		if(clearFlags_ != CLEAR_NONE)
		{
			graphics_.Clear(nullptr, clearColor_, clearFlags_, clearDepth_, clearStencil_);
		}

		OnRender(view);
	}

	void RenderPass::OnUpdate()
	{
	}

	void RenderPass::OnRender(View* view)
	{
	}

	ScenePass::ScenePass(unsigned clearFlags) : RenderPass(CMD_SCENEPASS)
	{
		clearFlags_ = clearFlags;
	}
	
	void ScenePass::OnRender(View* view)
	{
		auto& batchQueues = RenderContext(view->batchQueues_);
		auto it = batchQueues.find(passIndex_);
		if (it != batchQueues.end())
		{
			it->second.Draw(view);
		}
	}
}