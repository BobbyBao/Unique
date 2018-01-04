#pragma once

namespace Unique
{
	struct Impl;

	class GUISystem : public Object
	{
	public:
		GUISystem();
		~GUISystem();
	private:
		void HandleBeginFrame(const struct BeginFrame& eventData);
		Impl& impl_;
	};
}
