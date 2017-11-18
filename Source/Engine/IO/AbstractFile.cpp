
#include "../Precompiled.h"
#include "../IO/AbstractFile.h"

namespace Unique
{ 
	/// Construct.
	AbstractFile::AbstractFile() { }

	/// Construct.
	AbstractFile::AbstractFile(unsigned int size) : size_(size)
	{
	}

	/// Destruct.
	AbstractFile::~AbstractFile()
	{
	}

	bool AbstractFile::Read(String& str)
	{
		str.clear();

		while (!IsEof())
		{
			char c;
			Read<char>(c);
			if (!c)
				break;
			else
				str += c;
		}

		return true;
	}

	String AbstractFile::ReadFileID()
	{
		String ret;
		ret.Resize(4);
		Read(&ret[0], 4);
		return ret;
	}

	String AbstractFile::ReadLine()
	{
		String ret;

		while (!IsEof())
		{
			char c;
			Read(c);
			if (c == 10)
				break;
			if (c == 13)
			{
				// Peek next char to see if it's 10, and skip it too
				if (!IsEof())
				{
					char next;
					Read(next);
					if (next != 10)
						Seek(position_ - 1);
				}
				break;
			}

			ret += c;
		}

		return ret;
	}

	bool AbstractFile::Write(const String& value)
	{
		const char* chars = value.CString();
		// Count length to the first zero, because ReadString() does the same
		unsigned length = String::CStringLength(chars);
		return Write(chars, length + 1) == length + 1;
	}

	bool AbstractFile::WriteFileID(const String& value)
	{
		bool success = true;
		unsigned length = std::min(value.Length(), 4U);

		success &= Write(value.CString(), length) == length;
		for (unsigned i = value.Length(); i < 4; ++i)
			success &= Write<char>(' ');
		return success;
	}

	bool AbstractFile::WriteLine(const String& value)
	{
		bool success = true;
		success &= Write(value.CString(), value.Length()) == value.Length();
		success &= Write<byte>(13);
		success &= Write<byte>(10);
		return success;
	}
}
