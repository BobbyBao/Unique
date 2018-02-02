#pragma once
#include "TypeTraits.h"
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <set>
#include <list>
#include <unordered_map>

namespace Unique
{
	uPrimitiveTraits(std::string)

	template<class T, class Allocator>
	class TypeTraits<std::vector<T, Allocator> > : public SerializeTraitsArray<std::vector<T, Allocator> >
	{
	};

	template<class Allocator>
	class TypeTraits<std::vector<unsigned char, Allocator> > : public SerializeTraitsArray<std::vector<unsigned char, Allocator> >
	{
	};

	template<class T, class Allocator>
	class TypeTraits<std::deque<T, Allocator> > : public SerializeTraitsArray<std::deque<T, Allocator>, false >
	{
	};

	template<class T, class Allocator>
	class TypeTraits<std::list<T, Allocator> > : public SerializeTraitsArray<std::list<T, Allocator>, false >
	{
	public:
		static void ResizeSTLStyleArray(value_type& data, int rs) { data.resize(rs); }
	};

	template<class FirstClass, class SecondClass>
	class TypeTraits<std::pair<FirstClass, SecondClass> > : public TypeTraitsBase<std::pair<FirstClass, SecondClass> >
	{
	public:
		typedef std::pair<FirstClass, SecondClass>	value_type;

		template<class VisitFunction>
		inline static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitAttribute("Key", data.first, AF_DEFAULT);
			visitor.VisitAttribute("Value", data.second, AF_DEFAULT);
		}
	};

	template<class FirstClass, class SecondClass, class Compare, class Allocator>
	class TypeTraits<std::map<FirstClass, SecondClass, Compare, Allocator> > : public SerializeTraitsMap<std::map<FirstClass, SecondClass, Compare, Allocator> >
	{
	public:
		typedef std::map<FirstClass, SecondClass, Compare, Allocator>	value_type;

		template<class VisitFunction> inline
		static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitMap(data);
		}
	};

	template<class FirstClass, class SecondClass, class Compare, class Allocator>
	class TypeTraits<std::unordered_map<FirstClass, SecondClass, Compare, Allocator> > : public SerializeTraitsMap<std::map<FirstClass, SecondClass, Compare, Allocator> >
	{
	public:
		typedef std::unordered_map<FirstClass, SecondClass, Compare, Allocator>	value_type;
		
		template<class VisitFunction> inline
		static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitMap(data);
		}
	};

	template<class FirstClass, class SecondClass, class Compare, class Allocator>
	class TypeTraits<std::multimap<FirstClass, SecondClass, Compare, Allocator> > : public SerializeTraitsMap<std::multimap<FirstClass, SecondClass, Compare, Allocator> >
	{
	public:
		typedef std::multimap<FirstClass, SecondClass, Compare, Allocator>	value_type;

		template<class VisitFunction>
		inline static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitMap(data);
		}
	};


	template<class T, class Compare, class Allocator>
	class TypeTraits<std::set<T, Compare, Allocator> > : public SerializeTraitsArray<std::set<T, Compare, Allocator> >
	{
	public:
		template<class VisitFunction> 
		inline static void Visit(value_type& data, VisitFunction& visitor)
		{
			visitor.VisitSet(data);
		}
	};
	
	template<class T>
	struct NonConstContainerValueType<std::set<T> >
	{
		typedef T value_type;
	};

	template<class T0, class T1, class Compare, class Allocator>
	struct NonConstContainerValueType<std::map<T0, T1, Compare, Allocator> >
	{
		typedef std::pair<T0, T1> value_type;
	};

	template<class T0, class T1, class Compare, class Allocator>
	struct NonConstContainerValueType<std::multimap<T0, T1, Compare, Allocator> >
	{
		typedef std::pair<T0, T1> value_type;
	};


}
