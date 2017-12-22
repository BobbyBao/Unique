#pragma once
#include "../Core/Object.h"
#include "IStream.h"

namespace Unique
{
	class MemoryStream : public Object, public IStream
	{
		uRTTI(MemoryStream, Object)
	public:
		MemoryStream(UPtr<byte[]>&& data, unsigned size);
		MemoryStream(unsigned size);
		~MemoryStream();

		/// Read bytes from the file. Return number of bytes actually read.
		virtual unsigned Read(void* dest, unsigned size);
		/// Set position from the beginning of the file.
		virtual unsigned Seek(unsigned position);
		/// Write bytes to the file. Return number of bytes actually written.
		virtual unsigned Write(const void* data, unsigned size);

		virtual unsigned GetChecksum();
	protected:
		UPtr<byte[]> buffer_;
		/// Open mode.
		FileMode mode_;
		/// Content checksum.
		unsigned checksum_ = 0;
	};

}

