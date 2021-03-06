//
// Copyright (c) 2008-2017 the Urho3D project.
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

namespace Unique
{

	/// File open mode.
	enum FileMode
	{
		FILE_READ = 0,
		FILE_WRITE,
		FILE_READWRITE
	};

	class UNIQUE_API IStream
	{
	public:
		/// Construct.
		IStream();
		/// Construct.
		IStream(unsigned int size);
		/// Destruct.
		virtual ~IStream();

		/// Read bytes from the stream. Return number of bytes actually read.
		virtual unsigned Read(void* dest, unsigned size) = 0;
		/// Set position from the beginning of the stream.
		virtual unsigned Seek(unsigned position) = 0;
		/// Return name of the stream.
		virtual const String& GetName() const;
		/// Return a checksum if applicable.
		virtual unsigned GetChecksum();
		/// Write bytes to the stream. Return number of bytes actually written.
		virtual unsigned Write(const void* data, unsigned size) = 0;

		/// Return whether the end of stream has been reached.
		virtual bool IsEof() const { return position_ >= size_; }

		/// Return current position.
		unsigned GetPosition() const { return position_; }

		/// Return size.
		unsigned GetSize() const { return size_; }

		template<class T>
		T Read()
		{
			T value;
			int sz = Read(&value, sizeof value);
			assert(sz == sizeof value);
			return value;
		}

		inline byte ReadByte() 
		{
			return Read<byte>();
		}

		String ReadString();

		ByteArray ReadBytes();

		String ReadFileID();

		String ReadLine();

		ByteArray ReadAll();

		String ReadAllText();

		inline byte Peek()
		{
			unsigned pos = GetPosition();
			byte byte = ReadByte();
			Seek(pos);
			return byte;
		}
	
		template<class T>
		bool Write(const T& value)
		{
			return Write(&value, sizeof value) == sizeof value;
		}

		bool Write(const String& value);

		bool WriteFileID(const String& value);

		bool WriteLine(const String& value);

		bool Write(const ByteArray& bytes);
	protected:
		/// Stream position.
		unsigned position_;
		/// Stream size.
		unsigned size_;
	};


};
