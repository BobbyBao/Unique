#pragma once

namespace Unique
{
	enum class TransferState
	{
		Reading,
		Writing
	};

#define uSerializer(clss, state)\
public:\
		clss() : state_(state)\
		{\
		}\
		bool IsReading() const { return state_ == Unique::TransferState::Reading;}\
		bool IsWriting() const { return state_ == Unique::TransferState::Writing; }\
		bool BeginObject(int attrNum)\
		{\
			return BeginMap(attrNum);\
		}\
		void EndObject()\
		{\
			EndMap();\
		}\
		template<class T>\
		void TransferProperty(T& data, const char* name, int metaFlag = 0)\
		{\
			metaFlag_ = metaFlag;\
			if (BeginProperty(name))\
			{\
				Unique::SerializeTraits<T>::Transfer(data, *this);\
				EndProperty();\
			}\
		}\
		template <typename First, typename... Rest>\
		void TransferNVP(First& first, Rest&... rest)\
		{\
			int sz = sizeof ...(Rest)+1;\
			BeginMap(sz / 2);\
			TransferImpl1(first, rest...);\
			EndMap();\
		}\
	protected:\
		template <typename First, typename... Rest>\
		void TransferImpl1(First& first, Rest&... rest)\
		{\
			TransferImpl2(first, rest...);\
		}\
		template <typename First, typename Second, typename... Rest>\
		void TransferImpl2(First& val, Second name, const Rest&... rest)\
		{\
			TransferProperty(val, name);\
			TransferImpl1(rest...);\
		}\
		template <typename First, typename Second, typename... Rest>\
		void TransferImpl2(First& val, Second name)\
		{\
			TransferProperty(val, name);\
		}\
		int metaFlag_;\
		Unique::TransferState state_;\


}
