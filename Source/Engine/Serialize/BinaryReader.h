#pragma once
#include "../serialize/SerializeTraits.h"

namespace Unique
{
	class BinaryReader : public Reader
	{
	public:
		BinaryReader();
		~BinaryReader();

		template<class T>
		bool Load(const String& fileName, T& data);

		template<class T>
		bool Load(Deserializer& source, T& data);

		template<class T>
		void Transfer(T& data, const char* name, int metaFlag = 0);

		template<class T>
		void Transfer(T& data);

		template<class T>
		void TransferBasicData(T& data);

		template<class T>
		void TransferObject(SPtr<T>& data);

		template<class T>
		void TransferSTLStyleArray(T& data, int metaFlag = 0);

		template<class T>
		void TransferSTLStyleMap(T& data, int metaFlag = 0);

		template<class T>
		void TransferSTLStyleSet(T& data, int metaFlag = 0);
	protected:
		int metaFlag_;
	};

	#include "BinaryReader.inl"
}
