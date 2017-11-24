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
		void TransferAttribute(const char* name, T& data, int metaFlag = 0)\
		{\
			metaFlag_ = metaFlag;\
			if (StartAttribute(name))\
			{\
				Unique::SerializeTraits<T>::Transfer(data, *this);\
				EndAttribute();\
			}\
		}\
		template <typename... Rest>\
		void TransferAttributes(Rest&... rest)\
		{\
			int sz = sizeof ...(Rest);\
			StartObject(sz / 2);\
			TransferImpl1(rest...);\
			EndObject();\
		}\
		template<class T>\
		void Transfer(T& data)\
		{\
			data.Transfer(*this); \
		}\
	protected:\
		template <typename First, typename... Rest>\
		void TransferImpl1(First first, Rest&... rest)\
		{\
			TransferImpl2(first, rest...);\
		}\
		template <typename First, typename Second, typename... Rest>\
		void TransferImpl2(First name, Second& val, Rest&... rest)\
		{\
			TransferAttribute(name, val);\
			TransferImpl1(rest...);\
		}\
		template <typename First, typename Second, typename... Rest>\
		void TransferImpl2(First name, Second& val)\
		{\
			TransferAttribute(name, val);\
		}\
		int metaFlag_;\
		Unique::TransferState state_;\


}
