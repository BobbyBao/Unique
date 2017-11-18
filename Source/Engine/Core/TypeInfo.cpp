#include "../Precompiled.h"
#include "TypeInfo.h"
#include "Object.h"

namespace Unique
{

	TypeInfo::TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo) :
		type_(typeName),
		baseTypeInfo_(baseTypeInfo)
	{
	}

	TypeInfo::~TypeInfo()
	{
	}

	bool TypeInfo::IsTypeOf(const StringID& type) const
	{
		const TypeInfo* current = this;
		while (current)
		{
			if (current->GetType() == type)
				return true;

			current = current->GetBaseTypeInfo();
		}

		return false;
	}

	bool TypeInfo::IsTypeOf(const TypeInfo* typeInfo) const
	{
		const TypeInfo* current = this;
		while (current)
		{
			if (current == typeInfo)
				return true;

			current = current->GetBaseTypeInfo();
		}

		return false;
	}

	void TypeInfo::RegisterAttribute(Attribute* attr)
	{
		attributes_.push_back(std::move(UPtr<Attribute>(attr)));
	}

}

