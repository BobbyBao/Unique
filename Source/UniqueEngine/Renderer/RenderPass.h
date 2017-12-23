#pragma once

namespace Unique
{
	enum class RenderPassType
	{
		NONE = 0,
		CLEAR,
		SCENEPASS,
		QUAD,
		FORWARDLIGHTS,
		LIGHTVOLUMES,
		RENDERUI,
		SENDEVENT
	};

	enum class RenderPassSortMode
	{
		FRONTTOBACK = 0,
		BACKTOFRONT
	};

	class View;

	class RenderPass : public Object
	{
		uRTTI(RenderPass, Object)
	public:
		RenderPass();
		~RenderPass();

		virtual void Update(View* view);
		virtual void Render(View* view);

		/// Command type.
		RenderPassType type_;
		/// Sorting mode.
		RenderPassSortMode sortMode_;
		/// Scene pass name.
		String pass_;
		/// Scene pass index. Filled by View.
		unsigned passIndex_ = 0;

	protected:

	};

	class ClearPass : public RenderPass
	{
		uRTTI(ClearPass, RenderPass)
	public:
		virtual void Render(View* view);

	protected:
		/// Clear flags. Affects clear command only.
		unsigned clearFlags_;
		/// Clear color. Affects clear command only.
		Color clearColor_;
		/// Clear depth. Affects clear command only.
		float clearDepth_;
		/// Clear stencil value. Affects clear command only.
		unsigned clearStencil_;
	};

}
