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
	
	enum CLEAR_DEPTH_STENCIL_FLAGS : int
	{
		CLEAR_DEPTH_FLAG = 0x01,    ///< Clear depth part of the buffer
		CLEAR_STENCIL_FLAG = 0x02   ///< Clear stencil part of the buffer
	};

	class RenderPass
	{
	public:
		RenderPass(RenderPassType type);
		~RenderPass();
		
		/// Command type.
		RenderPassType type_;
		/// Sorting mode.
		RenderPassSortMode sortMode_;
		/// Scene pass name.
		String pass_;
		/// Scene pass index. Filled by View.
		unsigned passIndex_ = 0;
		
		/// Clear flags. Affects clear command only.
		unsigned clearFlags_ = CLEAR_DEPTH_FLAG;
		/// Clear color. Affects clear command only.
		Color clearColor_;
		/// Clear depth. Affects clear command only.
		float clearDepth_ = 1.0f;
		/// Clear stencil value. Affects clear command only.
		unsigned clearStencil_ = 0;
	protected:

	};
	
	class ClearPass : public RenderPass
	{
	public:
		ClearPass();

		virtual void Render(View* view);

	protected:
	};

}
