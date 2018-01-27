#include "Precompiled.h"
#include "Variant.h"

namespace Unique
{
	const Variant Variant::EMPTY;
	
	Variant::Variant(const Variant & other)
		: mContent(other.mContent ? other.mContent->clone() : 0)
	{
		if (other.isEmpty())
		{
			mType = VAR_NONE;
		}
		else if (other.isCustom())
		{
			mContent = other.mContent->clone();
		}
		else
		{
			memcpy(mData, other.mData, sizeof(mData));
		}
	}

	Variant::~Variant()
	{
		destroy();
	}

	void Variant::destroy()
	{
		if (!isEmpty())
		{
			if (isCustom())
			{
				delete mContent;
			}
			else
			{
				switch (mType)
				{
				case VAR_STRING:
					break;
				case VAR_BUFFER:
					break;
				default:
					break;
				}
			}

		}

	}
}