
#include "Precompiled.h"
#include "../IO/IStream.h"

namespace Unique
{ 
	IStream::IStream()
	{
	}

	IStream::IStream(unsigned int size) : size_(size)
	{
	}

	IStream::~IStream()
	{
	}

	const String& IStream::GetName() const
	{
		return String::EMPTY;
	}

	unsigned IStream::GetChecksum()
	{
		return 0;
	}

	String IStream::ReadString()
	{
		String str;

		while (!IsEof())
		{
			char c = Read<char>();
			if (!c)
				break;
			else
				str += c;
		}

		return str;
	}

	ByteArray IStream::ReadBytes()
	{
		ByteArray bytes;
		uint sz = Read<uint>();
		bytes.resize(sz);

		if(Read((void*)bytes.data(), sz) != sz)
			return ByteArray();
		
		return bytes;
	}

	String IStream::ReadFileID()
	{
		String ret;
		ret.Resize(4);
		Read(&ret[0], 4);
		return ret;
	}

	String IStream::ReadLine()
	{
		String ret;

		while (!IsEof())
		{
			char c = Read<char>();
			if (c == 10)
				break;
			if (c == 13)
			{
				// Peek next char to see if it's 10, and skip it too
				if (!IsEof())
				{
					char next = Read<char>();
					if (next != 10)
						Seek(position_ - 1);
				}
				break;
			}

			ret += c;
		}

		return ret;
	}

	ByteArray IStream::ReadAll()
	{
		uint size = GetSize();
		ByteArray byteArray(size);
		if (Read(byteArray.data(), size) != size)
		{
			return ByteArray();
		}

		return byteArray;
	}

	String IStream::ReadAllText()
	{
		uint size = GetSize();
		String str;
		str.Resize(size);
		if (Read(&str.At(0), size) != size)
		{
			return String();
		}

		return str;
	}

	bool IStream::Write(const String& value)
	{
		const char* chars = value.CString();
		// Count length to the first zero, because ReadString() does the same
		unsigned length = String::CStringLength(chars);
		return Write(chars, length + 1) == length + 1;
	}

	bool IStream::WriteFileID(const String& value)
	{
		bool success = true;
		unsigned length = std::min(value.Length(), 4U);

		success &= Write(value.CString(), length) == length;
		for (unsigned i = value.Length(); i < 4; ++i)
			success &= Write<char>(' ');
		return success;
	}

	bool IStream::WriteLine(const String& value)
	{
		bool success = true;
		success &= Write(value.CString(), value.Length()) == value.Length();
		success &= Write<byte>(13);
		success &= Write<byte>(10);
		return success;
	}


	bool IStream::Write(const ByteArray& bytes)
	{
		Write(bytes.size());
		return Write(bytes.data(), (uint)bytes.size()) == (uint)bytes.size();
		
	}
}
