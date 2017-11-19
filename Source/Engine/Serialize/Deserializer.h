#pragma once
#include "../serialize/SerializeTraits.h"

namespace Unique
{
	class Deserializer
	{
	public:

		bool IsReading() { return true; }
		bool IsWriting() { return false; }

		template<class T>
		bool Load(const String& fileName, T& data);

		template<class T>
		bool Load(Deserializer& source, T& data);

		template<class T>
		void Transfer(T& data, const char* name, int metaFlag = 0);


		void BeginMap(int sz)
		{
		}

		void EndMap()
		{
		}

		template <typename First, typename... Rest>
		void TransferNVP(First& first, Rest&... rest)
		{
			int sz = sizeof ...(Rest)+1;
			BeginMap(sz / 2);
			TransferImpl1(first, rest...); // recursive call using pack expansion syntax  
			EndMap();
		}

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

		template <typename First, typename... Rest>
		void TransferImpl1(First& first, Rest&... rest)
		{
			TransferImpl2(first, rest...); // recursive call using pack expansion syntax  
		}

		template <typename First, typename Second, typename... Rest>
		void TransferImpl2(First& val, Second name, const Rest&... rest)
		{
			Transfer(val, name);
			TransferImpl1(rest...); // recursive call using pack expansion syntax  
		}

		template <typename First, typename Second, typename... Rest>
		void TransferImpl2(First& val, Second name) {
			Transfer(val, name);
		}
		int metaFlag_;
	};

	#include "Deserializer.inl"
}
