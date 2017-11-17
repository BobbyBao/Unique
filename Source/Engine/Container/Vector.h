#pragma once

#include "../Container/VectorBase.h"

#include <cassert>
#include <cstring>
#include <new>
#include <initializer_list>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:6293)
#endif

namespace Unique
{

	/// %Vector template class.
	template <class T> class Vector : public VectorBase
	{
	public:
		typedef T value_type;
		typedef RandomAccessIterator<T> iterator;
		typedef RandomAccessConstIterator<T> const_iterator;

		/// Construct empty.
		Vector()
		{
		}

		/// Construct with initial size.
		explicit Vector(uint size)
		{
			resize(size);
		}

		/// Construct with initial size and default value.
		Vector(uint size, const T& value)
		{
			resize(size);
			for (uint i = 0; i < size; ++i)
				at(i) = value;
		}

		/// Construct with initial data.
		Vector(const T* data, uint size)
		{
			insert_elements(0, data, data + size);
		}

		/// Construct from another vector.
		Vector(const Vector<T>& vector)
		{
			*this = vector;
		}

		/// Aggregate initialization constructor.
		Vector(const std::initializer_list<T>& list) : Vector()
		{
			for (auto it = list.begin(); it != list.end(); it++)
			{
				push_back(*it);
			}
		}

		/// Destruct.
		~Vector()
		{
			destruct_elements(buffer(), size_);
			delete[] buffer_;
		}

		/// Assign from another vector.
		Vector<T>& operator =(const Vector<T>& rhs)
		{
			// In case of self-assignment do nothing
			if (&rhs != this)
			{
				clear();
				insert_elements(0, rhs.begin(), rhs.end());
			}
			return *this;
		}

		/// Add-assign an element.
		Vector<T>& operator +=(const T& rhs)
		{
			push_back(rhs);
			return *this;
		}

		/// Add-assign another vector.
		Vector<T>& operator +=(const Vector<T>& rhs)
		{
			push_back(rhs);
			return *this;
		}

		/// Add an element.
		Vector<T> operator +(const T& rhs) const
		{
			Vector<T> ret(*this);
			ret.push_back(rhs);
			return ret;
		}

		/// Add another vector.
		Vector<T> operator +(const Vector<T>& rhs) const
		{
			Vector<T> ret(*this);
			ret.push_back(rhs);
			return ret;
		}

		/// Test for equality with another vector.
		bool operator ==(const Vector<T>& rhs) const
		{
			if (rhs.size_ != size_)
				return false;

			T* buf = buffer();
			T* rhsBuffer = rhs.buffer();
			for (uint i = 0; i < size_; ++i)
			{
				if (buf[i] != rhsBuffer[i])
					return false;
			}

			return true;
		}

		/// Test for inequality with another vector.
		bool operator !=(const Vector<T>& rhs) const
		{
			if (rhs.size_ != size_)
				return true;

			T* buf = buffer();
			T* rhsBuffer = rhs.buffer();
			for (uint i = 0; i < size_; ++i)
			{
				if (buf[i] != rhsBuffer[i])
					return true;
			}

			return false;
		}

		/// Return element at index.
		T& operator [](uint index)
		{
			assert(index < size_);
			return buffer()[index];
		}

		/// Return const element at index.
		const T& operator [](uint index) const
		{
			assert(index < size_);
			return buffer()[index];
		}

		/// Return element at index.
		T& at(uint index)
		{
			assert(index < size_);
			return buffer()[index];
		}

		/// Return const element at index.
		const T& at(uint index) const
		{
			assert(index < size_);
			return buffer()[index];
		}

		/// Add an element at the end.
#ifndef COVERITY_SCAN_MODEL
		void push_back(const T& value)
		{
			insert_elements(size_, &value, &value + 1);
		}
#else
	// FIXME: Attempt had been made to use this model in the Coverity-Scan model file without any success
	// Probably because the model had generated a different mangled name than the one used by static analyzer
		void push_back(const T& value)
		{
			T array[] = { value };
			insert_elements(size_, array, array + 1);
		}
#endif

		/// Add another vector at the end.
		void push_back(const Vector<T>& vector) { insert_elements(size_, vector.begin(), vector.end()); }

		/// Remove the last element.
		void pop_back()
		{
			if (size_)
				resize(size_ - 1);
		}

		/// Insert an element at position.
		void insert(uint pos, const T& value)
		{
			insert_elements(pos, &value, &value + 1);
		}

		/// Insert another vector at position.
		void insert(uint pos, const Vector<T>& vector)
		{
			insert_elements(pos, vector.begin(), vector.end());
		}

		/// Insert an element by iterator.
		iterator insert(const iterator& dest, const T& value)
		{
			uint pos = (uint)(dest - begin());
			return insert_elements(pos, &value, &value + 1);
		}

		/// Insert a vector by iterator.
		iterator insert(const iterator& dest, const Vector<T>& vector)
		{
			uint pos = (uint)(dest - begin());
			return insert_elements(pos, vector.begin(), vector.end());
		}

		/// Insert a vector partially by iterators.
		iterator insert(const iterator& dest, const const_iterator& start, const const_iterator& end)
		{
			uint pos = (uint)(dest - begin());
			return insert_elements(pos, start, end);
		}

		/// Insert elements.
		iterator insert(const iterator& dest, const T* start, const T* end)
		{
			uint pos = (uint)(dest - begin());
			return insert_elements(pos, start, end);
		}

		/// Erase a range of elements.
		void erase(uint pos, uint length = 1)
		{
			// Return if the range is illegal
			if (pos + length > size_ || !length)
				return;

			move_range(pos, pos + length, size_ - pos - length);
			resize(size_ - length);
		}

		/// Erase a range of elements by swapping elements from the end of the array.
		void erase_swap(uint pos, uint length = 1)
		{
			uint shiftStartIndex = pos + length;
			// Return if the range is illegal
			if (shiftStartIndex > size_ || !length)
				return;

			uint newSize = size_ - length;
			uint trailingCount = size_ - shiftStartIndex;
			if (trailingCount <= length)
			{
				// We're removing more elements from the array than exist past the end of the range being removed, so perform a normal shift and destroy
				move_range(pos, shiftStartIndex, trailingCount);
			}
			else
			{
				// Swap elements from the end of the array into the empty space
				CopyElements(buffer() + pos, buffer() + newSize, length);
			}
			resize(newSize);
		}

		/// Erase an element by iterator. Return iterator to the next element.
		iterator erase(const iterator& it)
		{
			uint pos = (uint)(it - begin());
			if (pos >= size_)
				return end();
			erase(pos);

			return begin() + pos;
		}

		/// Erase a range by iterators. Return iterator to the next element.
		iterator erase(const iterator& start, const iterator& end)
		{
			uint pos = (uint)(start - begin());
			if (pos >= size_)
				return end();
			uint length = (uint)(end - start);
			erase(pos, length);

			return begin() + pos;
		}

		/// Erase an element by value. Return true if was found and erased.
		bool remove(const T& value)
		{
			iterator i = find(value);
			if (i != end())
			{
				erase(i);
				return true;
			}
			else
				return false;
		}

		/// Erase an element by value by swapping with the last element. Return true if was found and erased.
		bool remove_swap(const T& value)
		{
			iterator i = find(value);
			if (i != end())
			{
				erase_swap(i - begin());
				return true;
			}
			else
				return false;
		}

		/// Clear the vector.
		void clear() { resize(0); }

		/// Resize the vector.
		void resize(uint newSize) { Vector<T> tempBuffer; resize(newSize, 0, tempBuffer); }

		/// Resize the vector and fill new elements with default value.
		void resize(uint newSize, const T& value)
		{
			uint oldSize = Size();
			Vector<T> tempBuffer;
			resize(newSize, 0, tempBuffer);
			for (uint i = oldSize; i < newSize; ++i)
				at(i) = value;
		}

		/// Set new capacity.
		void reserve(uint newCapacity)
		{
			if (newCapacity < size_)
				newCapacity = size_;

			if (newCapacity != capacity_)
			{
				T* newBuffer = 0;
				capacity_ = newCapacity;

				if (capacity_)
				{
					newBuffer = reinterpret_cast<T*>(allocate_buffer((uint)(capacity_ * sizeof(T))));
					// Move the data into the new buffer
					construct_elements(newBuffer, buffer(), size_);
				}

				// Delete the old buffer
				destruct_elements(buffer(), size_);
				delete[] buffer_;
				buffer_ = reinterpret_cast<byte*>(newBuffer);
			}
		}

		/// Reallocate so that no extra memory is used.
		void Compact() { reserve(size_); }

		/// Return iterator to value, or to the end if not found.
		iterator find(const T& value)
		{
			iterator it = begin();
			while (it != end() && *it != value)
				++it;
			return it;
		}

		/// Return const iterator to value, or to the end if not found.
		const_iterator find(const T& value) const
		{
			const_iterator it = begin();
			while (it != end() && *it != value)
				++it;
			return it;
		}

		/// Return index of value in vector, or size if not found.
		uint index_of(const T& value) const
		{
			return Find(value) - begin();
		}

		/// Return whether contains a specific value.
		bool contains(const T& value) const { return find(value) != end(); }

		/// Return iterator to the beginning.
		iterator begin() { return iterator(buffer()); }

		/// Return const iterator to the beginning.
		const_iterator begin() const { return const_iterator(buffer()); }

		/// Return iterator to the end.
		iterator end() { return iterator(buffer() + size_); }

		/// Return const iterator to the end.
		const_iterator end() const { return const_iterator(buffer() + size_); }

		/// Return first element.
		T& front()
		{
			assert(size_);
			return buffer()[0];
		}

		/// Return const first element.
		const T& front() const
		{
			assert(size_);
			return buffer()[0];
		}

		/// Return last element.
		T& back()
		{
			assert(size_);
			return buffer()[size_ - 1];
		}

		/// Return const last element.
		const T& back() const
		{
			assert(size_);
			return buffer()[size_ - 1];
		}

		/// Return size of vector.
		uint size() const { return size_; }

		/// Return capacity of vector.
		uint capacity() const { return capacity_; }

		/// Return whether vector is empty.
		bool empty() const { return size_ == 0; }

		/// Return the buffer with right type.
		T* buffer() const { return reinterpret_cast<T*>(buffer_); }

	private:
		/// Resize the vector and create/remove new elements as necessary. Current buffer will be stored in tempBuffer in case of reallocation.
		void resize(uint newSize, const T* src, Vector<T>& tempBuffer)
		{
			// If size shrinks, destruct the removed elements
			if (newSize < size_)
				destruct_elements(buffer() + newSize, size_ - newSize);
			else
			{
				// Allocate new buffer if necessary and copy the current elements
				if (newSize > capacity_)
				{
					swap(tempBuffer);
					size_ = tempBuffer.size_;
					capacity_ = tempBuffer.capacity_;

					if (!capacity_)
						capacity_ = newSize;
					else
					{
						while (capacity_ < newSize)
							capacity_ += (capacity_ + 1) >> 1;
					}

					buffer_ = allocate_buffer((uint)(capacity_ * sizeof(T)));
					if (tempBuffer.buffer())
					{
						construct_elements(buffer(), tempBuffer.buffer(), size_);
					}
				}

				// Initialize the new elements
				construct_elements(buffer() + size_, src, newSize - size_);
			}

			size_ = newSize;
		}

		/// Insert elements.
		template <typename RandomIteratorT>
		iterator insert_elements(uint pos, RandomIteratorT start, RandomIteratorT end)
		{
			assert(start <= end);

			if (pos > size_)
				pos = size_;
			uint length = (uint)(end - start);
			Vector<T> tempBuffer;
			resize(size_ + length, 0, tempBuffer);
			move_range(pos + length, pos, size_ - pos - length);

			T* destPtr = buffer() + pos;
			for (RandomIteratorT it = start; it != end; ++it)
				*destPtr++ = *it;

			return begin() + pos;
		}

		/// Move a range of elements within the vector.
		void move_range(uint dest, uint src, uint count)
		{
			T* buf = buffer();
			if (src < dest)
			{
				for (uint i = count - 1; i < count; --i)
					buf[dest + i] = buf[src + i];
			}
			if (src > dest)
			{
				for (uint i = 0; i < count; ++i)
					buf[dest + i] = buf[src + i];
			}
		}

		/// Construct elements, optionally with source data.
		static void construct_elements(T* dest, const T* src, uint count)
		{
			if (!src)
			{
				for (uint i = 0; i < count; ++i)
					new(dest + i) T();
			}
			else
			{
				for (uint i = 0; i < count; ++i)
					new(dest + i) T(*(src + i));
			}
		}

		/// Copy elements from one buffer to another.
		static void CopyElements(T* dest, const T* src, uint count)
		{
			while (count--)
				*dest++ = *src++;
		}

		/// Call the elements' destructors.
		static void destruct_elements(T* dest, uint count)
		{
			while (count--)
			{
				dest->~T();
				++dest;
			}
		}
	};


/// %Vector template class for POD types. Does not call constructors or destructors and uses block move. Is intentionally (for performance reasons) unsafe for self-insertion.
template <class T> class PODVector : public VectorBase
{
public:
    typedef T value_type;
    typedef RandomAccessIterator<T> iterator;
    typedef RandomAccessConstIterator<T> const_iterator;

    /// Construct empty.
    PODVector()
    {
    }

    /// Construct with initial size.
    explicit PODVector(uint size)
    {
        resize(size);
    }

    /// Construct with initial size and default value.
    PODVector(uint size, const T& value)
    {
        resize(size);
        for (uint i = 0; i < size; ++i)
            at(i) = value;
    }

    /// Construct with initial data.
    PODVector(const T* data, uint size)
    {
        resize(size);
        copy_elements(buffer(), data, size);
    }

    /// Construct from another vector.
    PODVector(const PODVector<T>& vector)
    {
        *this = vector;
    }

    /// Aggregate initialization constructor.
    PODVector(const std::initializer_list<T>& list) : PODVector()
    {
        for (auto it = list.begin(); it != list.end(); it++)
        {
            push_back(*it);
        }
    }

    /// Destruct.
    ~PODVector()
    {
        delete[] buffer_;
    }

    /// Assign from another vector.
    PODVector<T>& operator =(const PODVector<T>& rhs)
    {
        // In case of self-assignment do nothing
        if (&rhs != this)
        {
            resize(rhs.size_);
            copy_elements(buffer(), rhs.buffer(), rhs.size_);
        }
        return *this;
    }

    /// Add-assign an element.
    PODVector<T>& operator +=(const T& rhs)
    {
        Push(rhs);
        return *this;
    }

    /// Add-assign another vector.
    PODVector<T>& operator +=(const PODVector<T>& rhs)
    {
        Push(rhs);
        return *this;
    }

    /// Add an element.
    PODVector<T> operator +(const T& rhs) const
    {
        PODVector<T> ret(*this);
        ret.push_back(rhs);
        return ret;
    }

    /// Add another vector.
    PODVector<T> operator +(const PODVector<T>& rhs) const
    {
        PODVector<T> ret(*this);
        ret.push_back(rhs);
        return ret;
    }

    /// Test for equality with another vector.
    bool operator ==(const PODVector<T>& rhs) const
    {
        if (rhs.size_ != size_)
            return false;

        T* buf = buffer();
        T* rhsBuffer = rhs.buffer();
        for (uint i = 0; i < size_; ++i)
        {
            if (buf[i] != rhsBuffer[i])
                return false;
        }

        return true;
    }

    /// Test for inequality with another vector.
    bool operator !=(const PODVector<T>& rhs) const
    {
        if (rhs.size_ != size_)
            return true;

        T* buffer = buffer();
        T* rhsBuffer = rhs.buffer();
        for (uint i = 0; i < size_; ++i)
        {
            if (buffer[i] != rhsBuffer[i])
                return true;
        }

        return false;
    }

    /// Return element at index.
    T& operator [](uint index)
    {
        assert(index < size_);
        return buffer()[index];
    }

    /// Return const element at index.
    const T& operator [](uint index) const
    {
        assert(index < size_);
        return buffer()[index];
    }

    /// Return element at index.
    T& at(uint index)
    {
        assert(index < size_);
        return buffer()[index];
    }

    /// Return const element at index.
    const T& at(uint index) const
    {
        assert(index < size_);
        return buffer()[index];
    }

    /// Add an element at the end.
    void push_back(const T& value)
    {
        if (size_ < capacity_)
            ++size_;
        else
            resize(size_ + 1);
        back() = value;
    }

    /// Add another vector at the end.
    void push_back(const PODVector<T>& vector)
    {
        uint oldSize = size_;
        resize(size_ + vector.size_);
        copy_elements(buffer() + oldSize, vector.buffer(), vector.size_);
    }

    /// Remove the last element.
    void pop_back()
    {
        if (size_)
            resize(size_ - 1);
    }

    /// Insert an element at position.
    void insert(uint pos, const T& value)
    {
        if (pos > size_)
            pos = size_;

        uint oldSize = size_;
        resize(size_ + 1);
        move_range(pos + 1, pos, oldSize - pos);
        buffer()[pos] = value;
    }

    /// Insert another vector at position.
    void insert(uint pos, const PODVector<T>& vector)
    {
        if (pos > size_)
            pos = size_;

        uint oldSize = size_;
        resize(size_ + vector.size_);
        move_range(pos + vector.size_, pos, oldSize - pos);
        copy_elements(buffer() + pos, vector.buffer(), vector.size_);
    }

    /// Insert an element by iterator.
    iterator insert(const iterator& dest, const T& value)
    {
        uint pos = (uint)(dest - begin());
        if (pos > size_)
            pos = size_;
        insert(pos, value);

        return begin() + pos;
    }

    /// Insert a vector by iterator.
    iterator insert(const iterator& dest, const PODVector<T>& vector)
    {
        uint pos = (uint)(dest - begin());
        if (pos > size_)
            pos = size_;
        insert(pos, vector);

        return begin() + pos;
    }

    /// Insert a vector partially by iterators.
    iterator insert(const iterator& dest, const const_iterator& start, const const_iterator& end)
    {
        uint pos = (uint)(dest - begin());
        if (pos > size_)
            pos = size_;
        uint length = (uint)(end - start);
        resize(size_ + length);
        move_range(pos + length, pos, size_ - pos - length);
        copy_elements(buffer() + pos, &(*start), length);

        return begin() + pos;
    }

    /// Insert elements.
    iterator insert(const iterator& dest, const T* start, const T* end)
    {
        uint pos = (uint)(dest - begin());
        if (pos > size_)
            pos = size_;
        uint length = (uint)(end - start);
        resize(size_ + length);
        move_range(pos + length, pos, size_ - pos - length);

        T* destPtr = buffer() + pos;
        for (const T* i = start; i != end; ++i)
            *destPtr++ = *i;

        return begin() + pos;
    }

    /// erase a range of elements.
    void erase(uint pos, uint length = 1)
    {
        // Return if the range is illegal
        if (!length || pos + length > size_)
            return;

        move_range(pos, pos + length, size_ - pos - length);
        resize(size_ - length);
    }

    /// erase an element by iterator. Return iterator to the next element.
    iterator erase(const iterator& it)
    {
        uint pos = (uint)(it - begin());
        if (pos >= size_)
            return end();
        erase(pos);

        return begin() + pos;
    }

    /// erase a range by iterators. Return iterator to the next element.
    iterator erase(const iterator& start, const iterator& end)
    {
        uint pos = (uint)(start - begin());
        if (pos >= size_)
            return end();
        uint length = (uint)(end - start);
        erase(pos, length);

        return begin() + pos;
    }

    /// erase a range of elements by swapping elements from the end of the array.
    void erase_swap(uint pos, uint length = 1)
    {
        uint shiftStartIndex = pos + length;
        // Return if the range is illegal
        if (shiftStartIndex > size_ || !length)
            return;

        uint newSize = size_ - length;
        uint trailingCount = size_ - shiftStartIndex;
        if (trailingCount <= length)
        {
            // We're removing more elements from the array than exist past the end of the range being removed, so perform a normal shift and destroy
            move_range(pos, shiftStartIndex, trailingCount);
        }
        else
        {
            // Swap elements from the end of the array into the empty space
            copy_elements(buffer() + pos, buffer() + newSize, length);
        }
        resize(newSize);
    }

    /// erase an element by value. Return true if was found and erased.
    bool remove(const T& value)
    {
        iterator i = find(value);
        if (i != end())
        {
            erase(i);
            return true;
        }
        else
            return false;
    }

    /// erase an element by value by swapping with the last element. Return true if was found and erased.
    bool remove_swap(const T& value)
    {
        iterator i = find(value);
        if (i != end())
        {
            erase_swap(i - begin());
            return true;
        }
        else
            return false;
    }

    /// Clear the vector.
    void clear() { resize(0); }

    /// Resize the vector.
    void resize(uint newSize)
    {
        if (newSize > capacity_)
        {
            if (!capacity_)
                capacity_ = newSize;
            else
            {
                while (capacity_ < newSize)
                    capacity_ += (capacity_ + 1) >> 1;
            }

            byte* newBuffer = allocate_buffer((uint)(capacity_ * sizeof(T)));
            // Move the data into the new buffer and delete the old
            if (buffer_)
            {
                copy_elements(reinterpret_cast<T*>(newBuffer), buffer(), size_);
                delete[] buffer_;
            }
            buffer_ = newBuffer;
        }

        size_ = newSize;
    }

    /// Set new capacity.
    void reserve(uint newCapacity)
    {
        if (newCapacity < size_)
            newCapacity = size_;

        if (newCapacity != capacity_)
        {
            byte* newBuffer = 0;
            capacity_ = newCapacity;

            if (capacity_)
            {
                newBuffer = allocate_buffer((uint)(capacity_ * sizeof(T)));
                // Move the data into the new buffer
                copy_elements(reinterpret_cast<T*>(newBuffer), buffer(), size_);
            }

            // Delete the old buffer
            delete[] buffer_;
            buffer_ = newBuffer;
        }
    }

    /// Reallocate so that no extra memory is used.
    void compact() { reserve(size_); }

    /// Return iterator to value, or to the end if not found.
    iterator find(const T& value)
    {
        iterator it = begin();
        while (it != end() && *it != value)
            ++it;
        return it;
    }

    /// Return const iterator to value, or to the end if not found.
    const_iterator find(const T& value) const
    {
        const_iterator it = begin();
        while (it != end() && *it != value)
            ++it;
        return it;
    }

    /// Return index of value in vector, or size if not found.
    uint index_Of(const T& value) const
    {
        return find(value) - begin();
    }

    /// Return whether contains a specific value.
    bool contains(const T& value) const { return find(value) != end(); }

    /// Return iterator to the beginning.
    iterator begin() { return iterator(buffer()); }

    /// Return const iterator to the beginning.
    const_iterator begin() const { return const_iterator(buffer()); }

    /// Return iterator to the end.
    iterator end() { return iterator(buffer() + size_); }

    /// Return const iterator to the end.
    const_iterator end() const { return const_iterator(buffer() + size_); }

    /// Return first element.
    T& front() { return buffer()[0]; }

    /// Return const first element.
    const T& front() const { return buffer()[0]; }

    /// Return last element.
    T& back()
    {
        assert(size_);
        return buffer()[size_ - 1];
    }

    /// Return const last element.
    const T& back() const
    {
        assert(size_);
        return buffer()[size_ - 1];
    }

    /// Return number of elements.
    uint size() const { return size_; }

    /// Return capacity of vector.
    uint capacity() const { return capacity_; }

    /// Return whether vector is empty.
    bool empty() const { return size_ == 0; }

    /// Return the buffer with right type.
    T* buffer() const { return reinterpret_cast<T*>(buffer_); }

private:
    /// Move a range of elements within the vector.
    void move_range(uint dest, uint src, uint count)
    {
        if (count)
            memmove(buffer() + dest, buffer() + src, count * sizeof(T));
    }

    /// Copy elements from one buffer to another.
    static void copy_elements(T* dest, const T* src, uint count)
    {
        if (count)
            memcpy(dest, src, count * sizeof(T));
    }
};

template <class T> typename Unique::PODVector<T>::const_iterator begin(const Unique::PODVector<T>& v) { return v.begin(); }

template <class T> typename Unique::PODVector<T>::const_iterator end(const Unique::PODVector<T>& v) { return v.end(); }

template <class T> typename Unique::PODVector<T>::iterator begin(Unique::PODVector<T>& v) { return v.begin(); }

template <class T> typename Unique::PODVector<T>::iterator end(Unique::PODVector<T>& v) { return v.end(); }

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
