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

#include <initializer_list>


namespace Unique
{

/// Singly-linked list node base class.
struct UNIQUE_API LinkedListNode
{
    /// Construct.
    LinkedListNode() :
        next_(0)
    {
    }

    /// Pointer to next node.
    LinkedListNode* next_;
};

/// Singly-linked list template class. Elements must inherit from LinkedListNode.
template <class T> class LinkedList
{
public:
    /// Construct empty.
    LinkedList() :
        head_(0)
    {
    }

    /// Aggregate initialization constructor.
    LinkedList(const std::initializer_list<T>& list) : LinkedList()
    {
        for (auto it = list.begin(); it != list.end(); it++)
        {
            insert(*it);
        }
    }

    /// Destruct.
    ~LinkedList()
    {
        clear();
    }

    /// Remove all elements.
    void clear()
    {
        T* element = head_;
        while (element)
        {
            T* n = next(element);
            delete element;
            element = n;
        }
    }

    /// Insert an element at the beginning.
    void insert_front(T* element)
    {
        if (element)
        {
            element->next_ = head_;
            head_ = element;
        }
    }

    /// Insert an element at the end.
    void insert(T* element)
    {
        if (head_)
        {
            T* tail = Last();
            element->next_ = tail->next_;
            tail->next_ = element;
        }
        else
        {
            element->next_ = head_;
            head_ = element;
        }
    }

    /// erase an element. Return true if successful.
    bool erase(T* element)
    {
        if (element && head_)
        {
            if (element == head_)
            {
                head_ = next(element);
                delete element;
                return true;
            }
            else
            {
                T* tail = head_;
                while (tail && tail->next_ != element)
                    tail = next(tail);
                if (tail)
                {
                    tail->next_ = element->next_;
                    delete element;
                    return true;
                }
            }
        }

        return false;
    }

    /// erase an element when the previous element is known (optimization.) Return true if successful.
    bool erase(T* element, T* previous)
    {
        if (previous && previous->next_ == element)
        {
            previous->next_ = element->next_;
            delete element;
            return true;
        }
        else if (!previous)
        {
            if (head_ == element)
            {
                head_ = next(element);
                delete element;
                return true;
            }
        }

        return false;
    }

    /// Return first element, or null if empty.
    T* first() const { return head_; }

    /// Return last element, or null if empty.
    T* last() const
    {
        T* element = head_;
        if (element)
        {
            while (element->next_)
                element = Next(element);
        }
        return element;
    }

    /// Return next element, or null if no more elements.
    T* next(T* element) const { return element ? static_cast<T*>(element->next_) : 0; }

    /// Return whether is empty.
    bool empty() const { return head_ == 0; }

private:
    /// First element.
    T* head_;
};

}
