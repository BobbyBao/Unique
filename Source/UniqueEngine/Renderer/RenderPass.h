#pragma once

namespace Unique
{
	enum RenderCommandType
	{
		CMD_NONE = 0,
		CMD_CLEAR,
		CMD_SCENEPASS,
		CMD_QUAD,
		CMD_FORWARDLIGHTS,
		CMD_LIGHTVOLUMES,
		CMD_RENDERUI,
		CMD_SENDEVENT
	};

	enum RenderCommandSortMode
	{
		SORT_FRONTTOBACK = 0,
		SORT_BACKTOFRONT
	};

	class RenderPass : public Object
	{
		uRTTI(RenderPass, Object)
	public:
		RenderPass();
		~RenderPass();

		virtual void Begin() {}
		virtual void End() {}

	protected:
		/// Command type.
		RenderCommandType type_;
		/// Sorting mode.
		RenderCommandSortMode sortMode_;
		/// Scene pass name.
		String pass_;
		/// Scene pass index. Filled by View.
		unsigned passIndex_;
	};


}
