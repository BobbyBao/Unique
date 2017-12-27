#pragma once
#include "resource.h"


namespace Unique
{
	class ResourceCache;

	class ResourceImporter : public Object
	{
	public:
		ResourceImporter(StringID type);
		~ResourceImporter();

		StringID GetResourceType() { return type_; }

		virtual SPtr<Resource> Import(const String& path);
	protected:
		ResourceCache& cache;
		StringID type_;
	};

	template<class T>
	class TResourceImporter : public ResourceImporter
	{
	public:
		TResourceImporter() : ResourceImporter(T::GetTypeStatic())
		{
		}
	};

}

