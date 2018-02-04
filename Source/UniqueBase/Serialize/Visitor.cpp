#include "Precompiled.h"
#include "Visitor.h"
#include "Core/TypeInfo.h"

namespace Unique
{
	Visitor::Visitor(VisitState state)
		: state_(state)
	{
	}

	bool Visitor::Visit(const StringID& type, void* obj)
	{
		TypeInfo* typeInfo = TypeInfo::GetTypeInfo(type);
		if (typeInfo)
		{
			typeInfo->Visit(*this, obj);
			return true;
		}

		return false;
	}

}
