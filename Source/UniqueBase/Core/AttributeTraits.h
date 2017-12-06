#pragma once

namespace Unique
{
	/// Attribute trait (default use const reference for object type).
	template <typename T> struct AttributeTrait
	{
		/// Get function return type.
		typedef const T& ReturnType;
		/// Set function parameter type.
		typedef const T& ParameterType;
	};

	/// Int attribute trait.
	template <> struct AttributeTrait<int>
	{
		typedef int ReturnType;
		typedef int ParameterType;
	};

	/// unsigned attribute trait.
	template <> struct AttributeTrait<unsigned>
	{
		typedef unsigned ReturnType;
		typedef unsigned ParameterType;
	};

	/// Bool attribute trait.
	template <> struct AttributeTrait<bool>
	{
		typedef bool ReturnType;
		typedef bool ParameterType;
	};

	/// Float attribute trait.
	template <> struct AttributeTrait<float>
	{
		typedef float ReturnType;
		typedef float ParameterType;
	};

	template <typename T>
	struct function_traits : function_traits< decltype(&T::operator()) > {};

	template<typename R, typename... Args>
	struct function_traits<R(Args...)>
	{
		static constexpr size_t arg_count = sizeof...(Args);

		using ReturnType = R;
		using ParameterType = R;
		using ArgTypes = std::tuple<Args...>;
	};

	template<typename R, typename... Args>
	struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> { };

	template<typename R, typename... Args>
	struct function_traits<R(&)(Args...)> : function_traits<R(Args...)> { };

	template<typename R, typename C, typename... Args>
	struct function_traits<R(C::*)(Args...)> : function_traits<R(Args...)>
	{
		using ClassType = C; 
	};

	template<typename R, typename C, typename... Args>
	struct function_traits<R(C::*)(Args...) const> : function_traits<R(Args...)>
	{ 
		using ClassType = C; 
		using RawType = R;
	};

	template<typename R, typename C, typename... Args>
	struct function_traits<const R& (C::*)(Args...) const> : function_traits<R(Args...)>
	{
		using ClassType = C;
		using RawType = R;
		using ReturnType = const R&;
		using ParameterType = const R&;
	};

	template<typename R, typename C, typename... Args>
	struct function_traits<R(C::*)(Args...) volatile> : function_traits<R(Args...)>
	{
		using ClassType = C;
	};

	template<typename R, typename C, typename... Args>
	struct function_traits<R(C::*)(Args...) const volatile> : function_traits<R(Args...)> { using ClassType = C; };

	template<typename T>
	struct function_traits<std::function<T>> : function_traits<T> {};

	/////////////////////////////////////////////////////////////////////////////////////
	// use it like e.g:
	// param_types<F, 0>::type

	template<typename F, size_t Index>
	struct param_types
	{
		using type = typename std::tuple_element<Index, typename function_traits<F>::arg_types>::type;
	};

	template<typename F, size_t Index>
	using param_types_t = typename param_types<F, Index>::type;

}
