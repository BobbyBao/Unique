#pragma once
#include "../Unique.h"

namespace Unique
{
	class String;

	class UNIQUE_API ByteArray
	{
	public:
		ByteArray();
		ByteArray(const char *);
		ByteArray(const char *, int size);
		ByteArray(const String& str);
		ByteArray(int size, char c);
		explicit ByteArray(int size);
		ByteArray(const ByteArray &);
		~ByteArray();

		ByteArray &operator=(const ByteArray &);
		ByteArray &operator=(const char *str);
		bool operator ==(const ByteArray& rhs);
		char at(int i) const;
		char operator[](int i) const;
		char& operator[](int i);

		int size() const;
		int count() const;
		int length() const;
		bool isNull() const;
		bool empty() const;
		void resize(int size);
		ByteArray &fill(char c, int size = -1);
		char *data();
		const char *data() const;
		void detach();
		bool isDetached() const;
		bool isSharedWith(const ByteArray &other) const;
		void clear();
		void truncate(int pos);
		void chop(int n);
		ByteArray trimmed() const;
		ByteArray simplified() const;
		ByteArray &setRawData(const char *data, uint size);
		ByteArray toBase64() const;	

		static ByteArray fromRawData(const char *data, int size);
		static ByteArray fromBase64(const ByteArray &base64);

	private:
		struct Data
		{
			uint ref;
			int alloc, size;
			char *data;
			char array[1];
		};

		static Data shared_null;
		static Data shared_empty;
		Data *d;
		ByteArray(Data *dd, int /*dummy*/, int /*dummy*/) 
			: d(dd) {}
		void realloc(int alloc);
		void expand(int i);
		ByteArray nulTerminated() const;

	public:
		typedef Data * DataPtr;
		inline DataPtr &data_ptr() { return d; }
	};

	inline ByteArray::ByteArray()
		: d(&shared_null)
	{ ++d->ref; }
	inline ByteArray::ByteArray(const ByteArray &a) : d(a.d)
	{ ++d->ref; }
	inline ByteArray::~ByteArray() 
	{ if (!--d->ref) free(d); }
	inline int ByteArray::size() const
	{ return d->size; }
	inline int ByteArray::count() const
	{ return d->size; }
	inline int ByteArray::length() const
	{ return d->size; }
	inline char ByteArray::at(int i) const
	{ assert(i >= 0 && i < size()); return d->data[i]; }
	inline char ByteArray::operator[](int i) const
	{ assert(i >= 0 && i < size()); return d->data[i]; }
	inline char& ByteArray::operator[](int i)
	{ assert(i < size()); return d->data[i]; }
	inline bool ByteArray::isNull() const 
	{ return d == &shared_null; }
	inline bool ByteArray::empty() const
	{ return d->size == 0; }
	inline char *ByteArray::data()
	{ detach(); return d->data; }
	inline const char *ByteArray::data() const
	{ return d->data; }
	inline void ByteArray::detach()
	{ if (d->ref != 1 || d->data != d->array) realloc(d->size); }
	inline bool ByteArray::isDetached() const
	{ return d->ref == 1; }
	inline bool ByteArray::isSharedWith(const ByteArray &other) const 
	{ return d == other.d; }

}
