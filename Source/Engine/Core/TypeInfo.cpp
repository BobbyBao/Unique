#include "../Precompiled.h"
#include "TypeInfo.h"
#include "Object.h"

namespace Unique
{

	static	HashMap<StringID, const TypeInfo*> typeInfoTbl_;
	static TypeInfo* sTypeInfo_[256];
	static int sTypeInfoCount_ = 0;
	static bool sInited_ = false;

	TypeInfo::TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo) :
		type_(typeName),
		baseTypeInfo_(baseTypeInfo)
	{
		if (sInited_)
		{
			typeInfoTbl_[type_] = this;
		}

		sTypeInfo_[sTypeInfoCount_++] = this;
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

	int TypeInfo::GetTypeInfo(TypeInfo** typeInfoList)
	{
		typeInfoList = sTypeInfo_;
		return sTypeInfoCount_;
	}

	void TypeInfo::Init()
	{
		sInited_ = true;

		for (int i = 0; i < sTypeInfoCount_; i++)
		{
			TypeInfo* typeInfo = sTypeInfo_[i];
			typeInfoTbl_[typeInfo->type_] = typeInfo;
		}
	}

	void TypeInfo::RegisterAttribute(Attribute* attr)
	{
		attributies_.push_back(std::move(UPtr<Attribute>(attr)));
	}

}

