#pragma once
#include "resource.h"


namespace Unique
{
	class ResourceReader : public Object
	{
	public:
		ResourceReader(StringID type);
		~ResourceReader();

		StringID resourceType() { return type_; }

		virtual SPtr<Resource> load(const String& path);
	protected:
		StringID type_;
	};

}

