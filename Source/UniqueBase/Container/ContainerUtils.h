//
// Copyright (c) 2008-2017 the Unique project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once
#include "../Unique.h"
#include <algorithm>

namespace Unique
{

template<typename T, typename U> constexpr size_t OffsetOf(U T::*member)
{
	return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

template <typename T, int count> int LengthOf(const T(&)[count])
{
	return count;
}

/// Swap two values.
template <class T> inline void Swap(T& first, T& second)
{
    T temp = first;
    first = second;
    second = temp;
}

template<class _Ty1, class _Ty2> 
inline auto MakePair(_Ty1&& _Val1, _Ty2&& _Val2)
{
	return std::make_pair(_Val1, _Val2);
}

template<class _Ty1, class _Ty2>
inline auto MakePair(const _Ty1& _Val1, const _Ty2& _Val2)
{
	return std::make_pair(_Val1, _Val2);
}

template<class C, class V>
typename C::iterator Find(C& c, const V& value)
{
	return std::find(c.begin(), c.end(), value);
}

template<class C, class V>
bool Contains(C& c, const V& value)
{
	return std::find(c.begin(), c.end(), value) != c.end();
}

template<class C, class V>
bool Remove(C& c, const V& value)
{
	auto i = Find(c, value);
	if (i != c.end())
	{
		c.erase(i);
		return true;
	}
	else
		return false;
}

template<class C, class V>
bool RemoveAll(C& c, const V& value)
{
	auto first = c.begin();
	auto last = c.end();
	auto result = first;
	while (first != last) {
		if (!(*first == val)) {
			*result = move(*first);
			++result;
		}
		++first;
	}
	return result;

}

template<class T>
void RemoveSwap(std::vector<T>& c, const T& value)
{
	auto it = std::find(c.begin(), c.end(), value);
	if (it != c.end())
	{
		*it = c.back();
		c.pop_back();
	}
}

template <class Iterator>
static void Sort(Iterator begin, Iterator end)
{
	std::sort(begin, end);
}

template <class Iterator, class Comparer>
static void Sort(Iterator begin, Iterator end, Comparer comparer)
{
	std::sort(begin, end, comparer);
}

template <class Iterator>
static void StableSort(Iterator begin, Iterator end)
{
	std::stable_sort(begin, end);
}

template <class Iterator, class Comparer>
static void StableSort(Iterator begin, Iterator end, Comparer comparer)
{
	std::stable_sort(begin, end, comparer);
}

template <class Iterator>
static void Reverse(Iterator begin, Iterator end)
{
	std::reverse(begin, end);
}

}
