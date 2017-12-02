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

	template<>
	class SerializeTraits<std::string> : public SerializeTraitsPrimitive<std::string>
	{
	public:
		static bool IsContinousMemoryArray() { return true; }
	};


	template<class T>
	inline void resize_trimmed(T& v, unsigned int sz)
	{
		// the vector is growing
		if (sz > v.size())
		{
			if (sz != v.capacity())
			{
				T temp(v.get_allocator());
				temp.reserve(sz);
				temp.assign(v.begin(), v.end());
				temp.resize(sz);
				temp.swap(v);
			}
			else
				v.resize(sz);
		}
		// the vector is shrinking
		else if (sz < v.size())
		{
			T temp(v.begin(), v.begin() + sz, v.get_allocator());
			temp.swap(v);
		}
	}

	template<class T, class Allocator>
	class SerializeTraits<std::vector<T, Allocator> > : public SerializeTraitsArray<std::vector<T, Allocator> >
	{
	public:
		
		template<class TransferFunction> 
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferArray(data);
		}

		static bool IsContinousMemoryArray() { return true; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { resize_trimmed(data, rs); }
	};

	template<class Allocator>
	class SerializeTraits<std::vector<unsigned char, Allocator> > : public SerializeTraitsArray<std::vector<unsigned char, Allocator> >
	{
	public:
				
		template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferArray(data);
		}

		static void ResizeSTLStyleArray(value_type& data, int rs) { resize_trimmed(data, rs); }
	};

	template<class T, class Allocator>
	class SerializeTraits<std::deque<T, Allocator> > : public SerializeTraitsArray<std::deque<T, Allocator>, false >
	{
	public:
		static void ResizeSTLStyleArray(value_type& data, int rs) { data.resize(rs); }
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
		inline static bool IsBasicType() { return true; }

		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.Transfer(data.first, "key", AttributeFlag::Default);
			transfer.Transfer(data.second, "value", AttributeFlag::Default);
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

	// Vector<bool> serialization is not allowed
	template<class Allocator>
	class SerializeTraits<std::vector<bool, Allocator> > : public SerializeTraitsBase<std::vector<bool, Allocator> >
	{
	public:
		// disallow std::vector<bool> serialization	
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
