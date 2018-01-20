#pragma once

namespace Unique
{
	enum ShaderDef
	{
		SKINNED,
	};


	class ShaderUtil
	{
	public:

		static unsigned GetPassIndex(const String& passName);
		static String GetShaderPath(DeviceType deviceType);
		static Vector<String> SplitDef(const String& defs);

		static unsigned basePassIndex;
		static unsigned alphaPassIndex;
		static unsigned materialPassIndex;
		static unsigned deferredPassIndex;
		static unsigned lightPassIndex;
		static unsigned litBasePassIndex;
		static unsigned litAlphaPassIndex;
		static unsigned shadowPassIndex;

		static const Vector<String> interDefs;
		inline static uint64 InterMask() { return (uint64)((1 << (interDefs.size() + 1)) - 1) << 32; }
	private:
		/// Pass index assignments.
		static HashMap<String, unsigned> passIndices;
	};
}
