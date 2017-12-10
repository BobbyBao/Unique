
#include "Precompiled.h"
#include "../IO/IFile.h"

namespace Unique
{ 
	IFile::IFile()
	{
	}

	IFile::IFile(unsigned int size) : size_(size)
	{
	}

	IFile::~IFile()
	{
	}

	const String& IFile::GetName() const
	{
		return String::EMPTY;
	}

	unsigned IFile::GetChecksum()
	{
		return 0;
	}

	String IFile::ReadString()
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

	ByteArray IFile::ReadBytes()
	{
		ByteArray bytes;
		uint sz = Read<uint>();
		bytes.resize(sz);

		if(Read((void*)bytes.data(), sz) != sz)
			return ByteArray();
		
		return bytes;
	}

	String IFile::ReadFileID()
	{
		String ret;
		ret.Resize(4);
		Read(&ret[0], 4);
		return ret;
	}

	String IFile::ReadLine()
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


	ByteArray IFile::ReadAll()
	{
		uint size = GetSize();
		ByteArray byteArray(size);
		if (Read(byteArray.data(), size) != size)
		{
			return ByteArray();
		}
		return byteArray;
	}

	bool IFile::Write(const String& value)
	{
		const char* chars = value.CString();
		// Count length to the first zero, because ReadString() does the same
		unsigned length = String::CStringLength(chars);
		return Write(chars, length + 1) == length + 1;
	}

	bool IFile::WriteFileID(const String& value)
	{
		bool success = true;
		unsigned length = std::min(value.Length(), 4U);

		success &= Write(value.CString(), length) == length;
		for (unsigned i = value.Length(); i < 4; ++i)
			success &= Write<char>(' ');
		return success;
	}

	bool IFile::WriteLine(const String& value)
	{
		bool success = true;
		success &= Write(value.CString(), value.Length()) == value.Length();
		success &= Write<byte>(13);
		success &= Write<byte>(10);
		return success;
	}


	bool IFile::Write(const ByteArray& bytes)
	{
		Write(bytes.size());
		return Write(bytes.data(), (uint)bytes.size()) == (uint)bytes.size();
		
	}
}
