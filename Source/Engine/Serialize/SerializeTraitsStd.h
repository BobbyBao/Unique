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
	class SerializeTraits<std::string> : public SerializeTraitsBase<std::string>
	{
	public:
		typedef std::string	value_type;
		inline static const char* GetTypeString(value_type*) { return "string"; }
		inline static bool IsBasicType() { return true; }

		template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferBasicData(data);
		}

		static bool IsContinousMemoryArray() { return true; }

		static void ResizeSTLStyleArray(value_type& data, int rs)
		{
			data.resize(rs, 1);
		}

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
	class SerializeTraits<std::vector<T, Allocator> > : public SerializeTraitsBase<std::vector<T, Allocator> >
	{
	public:

		typedef std::vector<T, Allocator>	value_type;
		DEFINE_GET_TYPESTRING_CONTAINER(vector)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferSTLStyleArray(data);
		}

		static bool IsContinousMemoryArray() { return true; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { resize_trimmed(data, rs); }
	};

	template<class Allocator>
	class SerializeTraits<std::vector<unsigned char, Allocator> > : public SerializeTraitsBase<std::vector<unsigned char, Allocator> >
	{
	public:

		typedef std::vector<unsigned char, Allocator>	value_type;

		inline static const char* GetTypeString(void*) { return "vector"; }
		inline static bool AllowTransferOptimization() { return false; }

		template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferSTLStyleArray(data);
			transfer.Align();
		}

		static bool IsContinousMemoryArray() { return true; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { resize_trimmed(data, rs); }
	};

	template<class T, class Allocator>
	class SerializeTraits<std::deque<T, Allocator> > : public SerializeTraitsBase<std::deque<T, Allocator> >
	{
	public:

		typedef std::deque<T, Allocator>	value_type;
		DEFINE_GET_TYPESTRING_CONTAINER(vector)

		template<class TransferFunction> inline
		static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferSTLStyleArray(data);
		}

		static bool IsContinousMemoryArray() { return false; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { data.resize(rs); }
	};

	template<class T, class Allocator>
	class SerializeTraits<std::list<T, Allocator> > : public SerializeTraitsBase<std::list<T, Allocator> >
	{
	public:

		typedef std::list<T, Allocator>	value_type;
		DEFINE_GET_TYPESTRING_CONTAINER(vector)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.TransferSTLStyleArray(data);
		}

		static bool IsContinousMemoryArray() { return false; }
		static void ResizeSTLStyleArray(value_type& data, int rs) { data.resize(rs); }
	};

	template<class FirstClass, class SecondClass>
	class SerializeTraits<std::pair<FirstClass, SecondClass> > : public SerializeTraitsBase<std::pair<FirstClass, SecondClass> >
	{
	public:

		typedef std::pair<FirstClass, SecondClass>	value_type;
		inline static const char* GetTypeString(void*) { return "pair"; }
		inline static bool AllowTransferOptimization() { return false; }
		inline static bool IsBasicType() { return true; }

		template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			transfer.Transfer(data.first, "key", TF_DEFAULT);
			transfer.Transfer(data.second, "value", TF_DEFAULT);
		}
	};

	template<class FirstClass, class SecondClass, class Compare, class Allocator>
	class SerializeTraits<std::map<FirstClass, SecondClass, Compare, Allocator> > : public SerializeTraitsBase<std::map<FirstClass, SecondClass, Compare, Allocator> >
	{
	public:

		typedef std::map<FirstClass, SecondClass, Compare, Allocator>	value_type;
		DEFINE_GET_TYPESTRING_MAP_CONTAINER(map)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			//		AssertIf(transfer.IsRemapPPtrTransfer() && SerializeTraits<FirstClass>::MightContainPPtr() && transfer.IsReadingPPtr());
			transfer.TransferSTLStyleMap(data);
		}
	};

	template<class FirstClass, class SecondClass, class Compare, class Allocator>
	class SerializeTraits<std::unordered_map<FirstClass, SecondClass, Compare, Allocator> > : public SerializeTraitsBase<std::map<FirstClass, SecondClass, Compare, Allocator> >
	{
	public:

		typedef std::unordered_map<FirstClass, SecondClass, Compare, Allocator>	value_type;
		DEFINE_GET_TYPESTRING_MAP_CONTAINER(map)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			//		AssertIf(transfer.IsRemapPPtrTransfer() && SerializeTraits<FirstClass>::MightContainPPtr() && transfer.IsReadingPPtr());
			transfer.TransferSTLStyleMap(data);
		}
	};

	template<class FirstClass, class SecondClass, class Compare, class Allocator>
	class SerializeTraits<std::multimap<FirstClass, SecondClass, Compare, Allocator> > : public SerializeTraitsBase<std::multimap<FirstClass, SecondClass, Compare, Allocator> >
	{
	public:

		typedef std::multimap<FirstClass, SecondClass, Compare, Allocator>	value_type;
		DEFINE_GET_TYPESTRING_MAP_CONTAINER(map)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			AssertIf(transfer.IsRemapPPtrTransfer() && SerializeTraits<FirstClass>::MightContainPPtr() && transfer.IsReadingPPtr());
			transfer.TransferSTLStyleMap(data);
		}
	};


	template<class T, class Compare, class Allocator>
	class SerializeTraits<std::set<T, Compare, Allocator> > : public SerializeTraitsBase<std::set<T, Compare, Allocator> >
	{
	public:

		typedef std::set<T, Compare, Allocator>	value_type;
		DEFINE_GET_TYPESTRING_CONTAINER(set)

			template<class TransferFunction> inline
			static void Transfer(value_type& data, TransferFunction& transfer)
		{
			AssertIf(transfer.IsRemapPPtrTransfer() && transfer.IsReadingPPtr());
			transfer.TransferSTLStyleMap(data);
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
