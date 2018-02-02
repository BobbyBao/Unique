#pragma once
#include <type_traits>

namespace Unique
{
	// TEMPLATE CLASS remove_reference
	template<class _Ty>
	struct remove_reference
	{	// remove reference
		typedef _Ty type;
	};

	template<class _Ty>
	struct remove_reference<_Ty&>
	{	// remove reference
		typedef _Ty type;
	};

	template<class _Ty>
	struct remove_reference<_Ty&&>
	{	// remove rvalue reference
		typedef _Ty type;
	};

	/// Attribute trait (default use const reference for object type).
	template <typename T> struct AttributeTrait
	{
		using RawType = typename remove_reference<T>::type;
		/// Get function return type.
		typedef const T& ReturnType;
		/// Set function parameter type.
		typedef const T& ParameterType;

		static ReturnType DEFAULT()
		{
			static RawType defaultVal;
			return defaultVal;
		}
	};
	
	/// Int attribute trait.
	template <> struct AttributeTrait<int>
	{
		typedef int ReturnType;
		typedef int ParameterType;
		static int DEFAULT(){ return 0; }
	};

	/// unsigned attribute trait.
	template <> struct AttributeTrait<unsigned>
	{
		typedef unsigned ReturnType;
		typedef unsigned ParameterType;
		static unsigned DEFAULT(){ return 0; }
	};

	/// Bool attribute trait.
	template <> struct AttributeTrait<bool>
	{
		typedef bool ReturnType;
		typedef bool ParameterType;
		static bool DEFAULT(){ return false; }
	};

	/// Float attribute trait.
	template <> struct AttributeTrait<float>
	{
		typedef float ReturnType;
		typedef float ParameterType;
		static float DEFAULT(){ return 0.0f; }
	};
	

	/// Mixed attribute trait (use const reference for set function only).
	template <typename T> struct MixedAttributeTrait
	{
		typedef T ReturnType;
		typedef const T& ParameterType;
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


	template <typename T>
	struct mixed_function_traits : mixed_function_traits< decltype(&T::operator()) > {};

	template<typename R, typename... Args>
	struct mixed_function_traits<R(Args...)>
	{
		static constexpr size_t arg_count = sizeof...(Args);

		using ReturnType = R;
		using ParameterType = const R&;
		using ArgTypes = std::tuple<Args...>;
	};

	template<typename R, typename C, typename... Args>
	struct mixed_function_traits<R(C::*)(Args...) const> : mixed_function_traits<R(Args...)>
	{
		using ClassType = C;
		using RawType = R;
	};

	template<typename R, typename C, typename... Args>
	struct mixed_function_traits<const R& (C::*)(Args...) const> : mixed_function_traits<R(Args...)>
	{
		using ClassType = C;
		using RawType = R;
		using ReturnType = const R&;
		using ParameterType = const R&;
	};
}
