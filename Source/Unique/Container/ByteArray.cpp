#include "Precompiled.h"
#include "ByteArray.h"


using namespace Unique;

int qAllocMore(int alloc, int extra)
{
	if (alloc == 0 && extra == 0)
		return 0;
	const int page = 1 << 12;
	int nalloc;
	alloc += extra;
	if (alloc < 1<<6) {
		nalloc = (1<<3) + ((alloc >>3) << 3);
	} else  {
		// don't do anything if the loop will overflow signed int.
		if (alloc >= INT_MAX/2)
			return INT_MAX;
		nalloc = (alloc < page) ? 1 << 3 : page;
		while (nalloc < alloc) {
			if (nalloc <= 0)
				return INT_MAX;
			nalloc *= 2;
		}
	}
	return nalloc - extra;
}

ByteArray::Data ByteArray::shared_null = {1, 0, 0, shared_null.array, {0} };
ByteArray::Data ByteArray::shared_empty = { 1, 0, 0, shared_empty.array, {0} };


ByteArray::ByteArray(const char *str)
{
	if (!str) {
		d = &shared_null;
	} else if (!*str) {
		d = &shared_empty;
	} else {
		size_t len = strlen(str);
		d = static_cast<Data *>(malloc(sizeof(Data)+len));
		d->ref = 0;;
		d->alloc = d->size = len;
		d->data = d->array;
		memcpy(d->array, str, len+1); // include null terminator
	}
	++d->ref;
}

ByteArray::ByteArray(const char *data, int size)
{
	if (!data) {
		d = &shared_null;
	} else if (size <= 0) {
		d = &shared_empty;
	} else {
		d = static_cast<Data *>(malloc(sizeof(Data) + size));
		d->ref = 0;
		d->alloc = d->size = size;
		d->data = d->array;
		memcpy(d->array, data, size);
		d->array[size] = '\0';
	}
	++d->ref;
}

ByteArray::ByteArray(const String& str)
{
	if (str.Length() <= 0) {
		d = &shared_empty;
	} else {
		d = static_cast<Data *>(malloc(sizeof(Data) + str.Length()));
		d->ref = 0;
		d->alloc = d->size = str.Length();
		d->data = d->array;
		memcpy(d->array, str.CString(), str.Length());
		d->array[str.Length()] = '\0';
	}
	++d->ref;
}

ByteArray::ByteArray(int size, char ch)
{
	if (size <= 0) {
		d = &shared_null;
	} else {
		d = static_cast<Data *>(malloc(sizeof(Data)+size));
		d->ref = 0;
		d->alloc = d->size = size;
		d->data = d->array;
		d->array[size] = '\0';
		memset(d->array, ch, size);
	}

	++d->ref;
}

ByteArray::ByteArray(int size)
{
	d = static_cast<Data *>(malloc(sizeof(Data)+size));
	d->ref = 1;
	d->alloc = d->size = size;
	d->data = d->array;
	d->array[size] = '\0';
}

ByteArray &ByteArray::operator=(const ByteArray & other)
{
	++other.d->ref;
	if (!--d->ref)
		free(d);
	d = other.d;
	return *this;
}

ByteArray &ByteArray::operator=(const char *str)
{
	Data *x;
	if (!str) {
		x = &shared_null;
	} else if (!*str) {
		x = &shared_empty;
	} else {
		int len = strlen(str);
		if (d->ref != 1 || len > d->alloc || (len < d->size && len < d->alloc >> 1))
			realloc(len);
		x = d;
		memcpy(x->data, str, len + 1); // include null terminator
		x->size = len;
	}
	++x->ref;
	if (!--d->ref)
		free(d);
	d = x;
	return *this;
}

bool ByteArray::operator ==(const ByteArray& rhs)
{
	return data() == rhs.data() && size() == rhs.size();
}

void ByteArray::truncate(int pos)
{
	if (pos < d->size)
		resize(pos);
}

void ByteArray::chop(int n)
{
	if (n > 0)
		resize(d->size - n);
}

void ByteArray::resize(int size)
{
	if (size <= 0) {
		Data *x = &shared_empty;
		++x->ref;
		if (!--d->ref)
			free(d);
		d = x;
	} else if (d == &shared_null) {
		//
		// Optimize the idiom:
		//    ByteArray a;
		//    a.resize(sz);
		//    ...
		// which is used in place of the Qt 3 idiom:
		//    ByteArray a(sz);
		//
		Data *x = static_cast<Data *>(malloc(sizeof(Data)+size));

		x->ref = 1;
		x->alloc = x->size = size;
		x->data = x->array;
		x->array[size] = '\0';
		(void) --d->ref; // cannot be 0, x points to shared_null
		d = x;
	} else {
		if (d->ref != 1 || size > d->alloc || (size < d->size && size < d->alloc >> 1))
			realloc(qAllocMore(size, sizeof(Data)));
		if (d->alloc >= size) {
			d->size = size;
			if (d->data == d->array) {
				d->array[size] = '\0';
			}
		}
	}
}

ByteArray &ByteArray::fill(char ch, int size)
{
	resize(size < 0 ? d->size : size);
	if (d->size)
		memset(d->data, ch, d->size);
	return *this;
}

void ByteArray::realloc(int alloc)
{
	if (d->ref != 1 || d->data != d->array) {
		Data *x = static_cast<Data *>(malloc(sizeof(Data) + alloc));
		x->size = std::min(alloc, d->size);
		::memcpy(x->array, d->data, x->size);
		x->array[x->size] = '\0';
		x->ref = 1;
		x->alloc = alloc;
		x->data = x->array;
		if (!--d->ref)
			free(d);
		d = x;
	} else {
		Data *x = static_cast<Data *>(::realloc(d, sizeof(Data) + alloc));
		x->alloc = alloc;
		x->data = x->array;
		d = x;
	}
}

void ByteArray::clear()
{
	if (!--d->ref)
		free(d);
	d = &shared_null;
	++d->ref;
}

ByteArray ByteArray::simplified() const
{
	if (d->size == 0)
		return *this;
	ByteArray result(d->size);
	const char *from = d->data;
	const char *fromend = from + d->size;
	int outc=0;
	char *to = result.d->data;
	for (;;) {
		while (from!=fromend && isspace(byte(*from)))
			from++;
		while (from!=fromend && !isspace(byte(*from)))
			to[outc++] = *from++;
		if (from!=fromend)
			to[outc++] = ' ';
		else
			break;
	}
	if (outc > 0 && to[outc-1] == ' ')
		outc--;
	result.resize(outc);
	return result;
}

ByteArray ByteArray::trimmed() const
{
	if (d->size == 0)
		return *this;
	const char *s = d->data;
	if (!isspace(byte(*s)) && !isspace(byte(s[d->size-1])))
		return *this;
	int start = 0;
	int end = d->size - 1;
	while (start<=end && isspace(byte(s[start])))  // skip white space from start
		start++;
	if (start <= end) {                          // only white space
		while (end && isspace(byte(s[end])))           // skip white space from end
			end--;
	}
	int l = end - start + 1;
	if (l <= 0) {
		++shared_empty.ref;
		return ByteArray(&shared_empty, 0, 0);
	}
	return ByteArray(s+start, l);
}

ByteArray ByteArray::toBase64() const
{
	const char alphabet[] = "ABCDEFGH" "IJKLMNOP" "QRSTUVWX" "YZabcdef"
		"ghijklmn" "opqrstuv" "wxyz0123" "456789+/";
	const char padchar = '=';
	int padlen = 0;

	ByteArray tmp((d->size * 4) / 3 + 3);

	int i = 0;
	char *out = tmp.data();
	while (i < d->size) {
		int chunk = 0;
		chunk |= int(byte(d->data[i++])) << 16;
		if (i == d->size) {
			padlen = 2;
		} else {
			chunk |= int(byte(d->data[i++])) << 8;
			if (i == d->size) padlen = 1;
			else chunk |= int(byte(d->data[i++]));
		}

		int j = (chunk & 0x00fc0000) >> 18;
		int k = (chunk & 0x0003f000) >> 12;
		int l = (chunk & 0x00000fc0) >> 6;
		int m = (chunk & 0x0000003f);
		*out++ = alphabet[j];
		*out++ = alphabet[k];
		if (padlen > 1) *out++ = padchar;
		else *out++ = alphabet[l];
		if (padlen > 0) *out++ = padchar;
		else *out++ = alphabet[m];
	}

	tmp.truncate(out - tmp.data());
	return tmp;
}

ByteArray ByteArray::fromRawData(const char *data, int size)
{
	Data *x = static_cast<Data *>(malloc(sizeof(Data)));

	if (data) {
		x->data = const_cast<char *>(data);
	} else {
		x->data = x->array;
		size = 0;
	}
	x->ref = 1;
	x->alloc = x->size = size;
	*x->array = '\0';
	return ByteArray(x, 0, 0);
}

ByteArray &ByteArray::setRawData(const char *data, uint size)
{
	if (d->ref != 1 || d->alloc) {
		*this = fromRawData(data, size);
	} else {
		if (data) {
			d->data = const_cast<char *>(data);
		} else {
			d->data = d->array;
			size = 0;
		}
		d->alloc = d->size = size;
		*d->array = '\0';
	}
	return *this;
}

ByteArray ByteArray::fromBase64(const ByteArray &base64)
{
	unsigned int buf = 0;
	int nbits = 0;
	ByteArray tmp((base64.size() * 3) / 4);

	int offset = 0;
	for (int i = 0; i < base64.size(); ++i) {
		int ch = base64.at(i);
		int d;

		if (ch >= 'A' && ch <= 'Z')
			d = ch - 'A';
		else if (ch >= 'a' && ch <= 'z')
			d = ch - 'a' + 26;
		else if (ch >= '0' && ch <= '9')
			d = ch - '0' + 52;
		else if (ch == '+')
			d = 62;
		else if (ch == '/')
			d = 63;
		else
			d = -1;

		if (d != -1) {
			buf = (buf << 6) | d;
			nbits += 6;
			if (nbits >= 8) {
				nbits -= 8;
				tmp[offset++] = buf >> nbits;
				buf &= (1 << nbits) - 1;
			}
		}
	}

	tmp.truncate(offset);
	return tmp;
}