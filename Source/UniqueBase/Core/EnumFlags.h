#pragma once
#include <type_traits>
template<typename Enum>
struct EnableBitMaskOperators
{
	static const bool enable = false;
};

#define uBitMask(x)  \
template<>                           \
struct EnableBitMaskOperators<x>     \
{                                    \
static const bool enable = true; \
};

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator |(Enum lhs, Enum rhs)
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum> (
		static_cast<underlying>(lhs) |
		static_cast<underlying>(rhs)
		);
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum&>::type
operator |=(Enum& lhs, Enum rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator &(Enum lhs, Enum rhs)
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<Enum> (
		static_cast<underlying>(lhs) &
		static_cast<underlying>(rhs)
		);
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator &=(Enum lhs, Enum rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, bool>::type
operator ||(Enum lhs, Enum rhs)
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<bool> (
		static_cast<underlying>(lhs) ||
		static_cast<underlying>(rhs)
		);
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, bool>::type
operator &&(Enum lhs, Enum rhs)
{
	using underlying = typename std::underlying_type<Enum>::type;
	return static_cast<bool> (
		static_cast<underlying>(lhs) &&
		static_cast<underlying>(rhs)
		);
}
