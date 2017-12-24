#include "Precompiled.h"
#include "TypeInfo.h"
#include "Object.h"
#include "../Core/Attribute.h"

namespace Unique
{
	static const int MAX_TYPES = 256;
	static TypeInfo* s_typeInfos[MAX_TYPES];
	static HashMap<StringID, TypeInfo*> s_typeInfoMap;
	static int s_typeCount = 0;

	TypeInfo::TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo) :
		type_(typeName),
		baseTypeInfo_(baseTypeInfo)
	{
		assert(s_typeCount < MAX_TYPES);
		s_typeInfos[s_typeCount++] = this;
	}

	TypeInfo::~TypeInfo()
	{
		for (auto it : attributes_)
		{
			delete it;
		}

		attributes_.clear();
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
		attributes_.push_back(attr);
	}

	int TypeInfo::GetAllTypeInfo(TypeInfo** typeInfos)
	{
		typeInfos = s_typeInfos;
		return s_typeCount;
	}

	void TypeInfo::RegisterTypeInfo()
	{
		for (int i = 0; i < s_typeCount; i++)
		{
			s_typeInfoMap[s_typeInfos[i]->GetType()] = s_typeInfos[i];
		}
		
	}

	TypeInfo* TypeInfo::GetTypeInfo(const StringID& type)
	{
		auto it = s_typeInfoMap.find(type);
		if (it != s_typeInfoMap.end())
		{
			return it->second;
		}
		return nullptr;
	}

	uExportStatic(TypeInfo, int, GetAllTypeInfo, TypeInfo**, typeInfos)

	uExportStatic(TypeInfo, TypeInfo*, GetTypeInfo, const StringID&, type)
	

}

