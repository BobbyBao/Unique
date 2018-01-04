#pragma once
#include "SerializeDefs.h"
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
	class SerializeTraits<std::vector<T, Allocator> > : public SerializeTraitsArray<std::vector<T, Allocator> >
	{
	};

	template<class Allocator>
	class SerializeTraits<std::vector<unsigned char, Allocator> > : public SerializeTraitsArray<std::vector<unsigned char, Allocator> >
	{
	};

	template<class T, class Allocator>
	class SerializeTraits<std::deque<T, Allocator> > : public SerializeTraitsArray<std::deque<T, Allocator>, false >
	{
	};

	template<class T, class Allocator>
	class SerializeTraits<std::list<T, Allocator> > : public SerializeTraitsArray<std::list<T, Allocator>, false >
	{
	public:
		static void ResizeSTLStyleArray(value_type& data, int rs) { data.resize(rs); }
	};

	template<class FirstClass, class SecondClass>
	class SerializeTraits<std::pair<FirstClass, SecondClass> > : public SerializeTraitsBase<std::pair<FirstClass, SecondClass> >
	{
	public:
		typedef std::pair<FirstClass, SecondClass>	value_type;

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferAttribute("Key", data.first, AttributeFlag::Default);
			transfer.TransferAttribute("Value", data.second, AttributeFlag::Default);
		}
	};

	template<class FirstClass, class SecondClass, class Compare, class Allocator>
	class SerializeTraits<std::map<FirstClass, SecondClass, Compare, Allocator> > : public SerializeTraitsMap<std::map<FirstClass, SecondClass, Compare, Allocator> >
	{
	public:
		typedef std::map<FirstClass, SecondClass, Compare, Allocator>	value_type;

		template<class TransferFunction> inline
		static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferMap(data);
		}
	};

	template<class FirstClass, class SecondClass, class Compare, class Allocator>
	class SerializeTraits<std::unordered_map<FirstClass, SecondClass, Compare, Allocator> > : public SerializeTraitsMap<std::map<FirstClass, SecondClass, Compare, Allocator> >
	{
	public:
		typedef std::unordered_map<FirstClass, SecondClass, Compare, Allocator>	value_type;
		
		template<class TransferFunction> inline
		static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferMap(data);
		}
	};

	template<class FirstClass, class SecondClass, class Compare, class Allocator>
	class SerializeTraits<std::multimap<FirstClass, SecondClass, Compare, Allocator> > : public SerializeTraitsMap<std::multimap<FirstClass, SecondClass, Compare, Allocator> >
	{
	public:
		typedef std::multimap<FirstClass, SecondClass, Compare, Allocator>	value_type;

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferMap(data);
		}
	};


	template<class T, class Compare, class Allocator>
	class SerializeTraits<std::set<T, Compare, Allocator> > : public SerializeTraitsArray<std::set<T, Compare, Allocator> >
	{
	public:
		template<class TransferFunction> 
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferSet(data);
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
