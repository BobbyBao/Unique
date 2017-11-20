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
		template<class T>\
		void TransferAttribute(T& data, const char* name, int metaFlag = 0)\
		{\
			metaFlag_ = metaFlag;\
			if (StartProperty(name))\
			{\
				Unique::SerializeTraits<T>::Transfer(data, *this);\
				EndProperty();\
			}\
		}\
		template <typename First, typename... Rest>\
		void TransferAttributes(First& first, Rest&... rest)\
		{\
			int sz = sizeof ...(Rest)+1;\
			StartObject(sz / 2);\
			TransferImpl1(first, rest...);\
			EndMap();\
		}\
		template<class T>\
		void Transfer(T& data)\
		{\
			data.Transfer(*this); \
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
			TransferAttribute(val, name);\
			TransferImpl1(rest...);\
		}\
		template <typename First, typename Second, typename... Rest>\
		void TransferImpl2(First& val, Second name)\
		{\
			TransferAttribute(val, name);\
		}\
		int metaFlag_;\
		Unique::TransferState state_;\


}
