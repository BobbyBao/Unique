#include "../Precompiled.h"
#include "TypeInfo.h"
#include "Object.h"

namespace Unique
{

	static	HashMap<StringID, const TypeInfo*> typeInfoTbl_;
	static TypeInfo* sTypeInfo_[256];
	static int sTypeInfoCount_ = 0;
	static bool sInited_ = false;
//	static RegisterRuntime sTypeInfoRuntime_(TypeInfo::Init);

	TypeInfo::TypeInfo(const char* typeName, const TypeInfo* baseTypeInfo, CreateObjectFn createObjectFn) :
		type_(typeName),
		baseTypeInfo_(baseTypeInfo),
		createObjectFn_(createObjectFn)
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


	SPtr<Object> TypeInfo::CreateObject() const
	{
		if (createObjectFn_)
		{
			return createObjectFn_();
		}

		return nullptr;
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

	SPtr<Object> CreateObject(const StringID& type)
	{
		auto it = typeInfoTbl_.find(type);
		if (it != typeInfoTbl_.end())
		{
			return it->second->CreateObject();
		}

		return SPtr<Object>();
	}

}

