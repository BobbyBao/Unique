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
class String;
class VectorBase;

/// Swap two values.
template <class T> inline void Swap(T& first, T& second)
{
    T temp = first;
    first = second;
    second = temp;
}

template <> UNIQUE_API void Swap<String>(String& first, String& second);

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
void Remove(C& c, const V& value)
{
	std::remove(c.begin(), c.end(), value);
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

}
