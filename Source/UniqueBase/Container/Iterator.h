#pragma once

namespace Unique
{


/// Random access iterator.
template <class T> struct RandomAccessIterator
{
	/// Construct.
	RandomAccessIterator() :
		ptr_(0)
	{
	}

	/// Construct with an object pointer.
	explicit RandomAccessIterator(T* ptr) :
		ptr_(ptr)
	{
	}

	/// Point to the object.
	T* operator ->() const { return ptr_; }

	/// Dereference the object.
	T& operator *() const { return *ptr_; }

	/// Preincrement the pointer.
	RandomAccessIterator<T>& operator ++()
	{
		++ptr_;
		return *this;
	}

	/// Postincrement the pointer.
	RandomAccessIterator<T> operator ++(int)
	{
		RandomAccessIterator<T> it = *this;
		++ptr_;
		return it;
	}

	/// Predecrement the pointer.
	RandomAccessIterator<T>& operator --()
	{
		--ptr_;
		return *this;
	}

	/// Postdecrement the pointer.
	RandomAccessIterator<T> operator --(int)
	{
		RandomAccessIterator<T> it = *this;
		--ptr_;
		return it;
	}

	/// Add an offset to the pointer.
	RandomAccessIterator<T>& operator +=(int value)
	{
		ptr_ += value;
		return *this;
	}

	/// Subtract an offset from the pointer.
	RandomAccessIterator<T>& operator -=(int value)
	{
		ptr_ -= value;
		return *this;
	}

	/// Add an offset to the pointer.
	RandomAccessIterator<T> operator +(int value) const { return RandomAccessIterator<T>(ptr_ + value); }

	/// Subtract an offset from the pointer.
	RandomAccessIterator<T> operator -(int value) const { return RandomAccessIterator<T>(ptr_ - value); }

	/// Calculate offset to another iterator.
	int operator -(const RandomAccessIterator& rhs) const { return (int)(ptr_ - rhs.ptr_); }

	/// Test for equality with another iterator.
	bool operator ==(const RandomAccessIterator& rhs) const { return ptr_ == rhs.ptr_; }

	/// Test for inequality with another iterator.
	bool operator !=(const RandomAccessIterator& rhs) const { return ptr_ != rhs.ptr_; }

	/// Test for less than with another iterator.
	bool operator <(const RandomAccessIterator& rhs) const { return ptr_ < rhs.ptr_; }

	/// Test for greater than with another iterator.
	bool operator >(const RandomAccessIterator& rhs) const { return ptr_ > rhs.ptr_; }

	/// Test for less than or equal with another iterator.
	bool operator <=(const RandomAccessIterator& rhs) const { return ptr_ <= rhs.ptr_; }

	/// Test for greater than or equal with another iterator.
	bool operator >=(const RandomAccessIterator& rhs) const { return ptr_ >= rhs.ptr_; }

	/// Pointer.
	T* ptr_;
};

/// Random access const iterator.
template <class T> struct RandomAccessConstIterator
{
	/// Construct.
	RandomAccessConstIterator() :
		ptr_(0)
	{
	}

	/// Construct with an object pointer.
	explicit RandomAccessConstIterator(T* ptr) :
		ptr_(ptr)
	{
	}

	/// Construct from a non-const iterator.
	RandomAccessConstIterator(const RandomAccessIterator<T>& rhs) :
		ptr_(rhs.ptr_)
	{
	}

	/// Assign from a non-const iterator.
	RandomAccessConstIterator<T>& operator =(const RandomAccessIterator<T>& rhs)
	{
		ptr_ = rhs.ptr_;
		return *this;
	}

	/// Point to the object.
	const T* operator ->() const { return ptr_; }

	/// Dereference the object.
	const T& operator *() const { return *ptr_; }

	/// Preincrement the pointer.
	RandomAccessConstIterator<T>& operator ++()
	{
		++ptr_;
		return *this;
	}

	/// Postincrement the pointer.
	RandomAccessConstIterator<T> operator ++(int)
	{
		RandomAccessConstIterator<T> it = *this;
		++ptr_;
		return it;
	}

	/// Predecrement the pointer.
	RandomAccessConstIterator<T>& operator --()
	{
		--ptr_;
		return *this;
	}

	/// Postdecrement the pointer.
	RandomAccessConstIterator<T> operator --(int)
	{
		RandomAccessConstIterator<T> it = *this;
		--ptr_;
		return it;
	}

	/// Add an offset to the pointer.
	RandomAccessConstIterator<T>& operator +=(int value)
	{
		ptr_ += value;
		return *this;
	}

	/// Subtract an offset from the pointer.
	RandomAccessConstIterator<T>& operator -=(int value)
	{
		ptr_ -= value;
		return *this;
	}

	/// Add an offset to the pointer.
	RandomAccessConstIterator<T> operator +(int value) const { return RandomAccessConstIterator<T>(ptr_ + value); }

	/// Subtract an offset from the pointer.
	RandomAccessConstIterator<T> operator -(int value) const { return RandomAccessConstIterator<T>(ptr_ - value); }

	/// Calculate offset to another iterator.
	int operator -(const RandomAccessConstIterator& rhs) const { return (int)(ptr_ - rhs.ptr_); }

	/// Test for equality with another iterator.
	bool operator ==(const RandomAccessConstIterator& rhs) const { return ptr_ == rhs.ptr_; }

	/// Test for inequality with another iterator.
	bool operator !=(const RandomAccessConstIterator& rhs) const { return ptr_ != rhs.ptr_; }

	/// Test for less than with another iterator.
	bool operator <(const RandomAccessConstIterator& rhs) const { return ptr_ < rhs.ptr_; }

	/// Test for greater than with another iterator.
	bool operator >(const RandomAccessConstIterator& rhs) const { return ptr_ > rhs.ptr_; }

	/// Test for less than or equal with another iterator.
	bool operator <=(const RandomAccessConstIterator& rhs) const { return ptr_ <= rhs.ptr_; }

	/// Test for greater than or equal with another iterator.
	bool operator >=(const RandomAccessConstIterator& rhs) const { return ptr_ >= rhs.ptr_; }

	/// Pointer.
	T* ptr_;
};

/// Returns an iterator pointing to the first element in the range [first, last) that is not less than value.
template <class TRandomAccessIterator, class T>
TRandomAccessIterator LowerBound(TRandomAccessIterator first, TRandomAccessIterator last, const T& value)
{
	unsigned count = last - first;

	while (count > 0)
	{
		const unsigned step = count / 2;
		const TRandomAccessIterator it = first + step;
		if (*it < value)
		{
			first = it + 1;
			count -= step + 1;
		}
		else
		{
			count = step;
		}
	}
	return first;
}

/// Returns an iterator pointing to the first element in the range [first, last) that is greater than value.
template <class TRandomAccessIterator, class T>
TRandomAccessIterator UpperBound(TRandomAccessIterator first, TRandomAccessIterator last, const T& value)
{
	unsigned count = last - first;

	while (count > 0)
	{
		const unsigned step = count / 2;
		const TRandomAccessIterator it = first + step;
		if (!(value < *it))
		{
			first = it + 1;
			count -= step + 1;
		}
		else
		{
			count = step;
		};
	}
	return first;
}


}