#include "Precompiled.h"
#include "MemoryStream.h"

namespace Unique
{
	MemoryStream::MemoryStream(UPtr<byte[]>&& data, unsigned size)
		: IStream(size), buffer_(std::move(data)), mode_(FILE_READ)
	{
	}

	MemoryStream::MemoryStream(unsigned size) 
		: IStream(size), buffer_(new byte[size]), mode_(FILE_WRITE)
	{
	}

	MemoryStream::~MemoryStream()
	{
	}

	unsigned MemoryStream::Read(void* dest, unsigned size)
	{
		if (mode_ == FILE_WRITE)
		{
			UNIQUE_LOGERROR("File not opened for reading");
			return 0;
		}

		if (size + position_ > size_)
			size = size_ - position_;

		if (!size)
			return 0;
				
		std::memcpy(dest, buffer_.get(), size);
		position_ += size;
		return size;
	}

	unsigned MemoryStream::Seek(unsigned position)
	{
		// Allow sparse seeks if writing
		if (mode_ == FILE_READ && position > size_)
			position = size_;

		position_ = position;
		return position_;
	}

	unsigned MemoryStream::Write(const void* data, unsigned size)
	{
		if (mode_ == FILE_READ)
		{
			UNIQUE_LOGERROR("File not opened for writing");
			return 0;
		}

		if (!size)
			return 0;

		std::memcpy(buffer_.get(), data, size);

		position_ += size;
		if (position_ > size_)
			size_ = position_;

		return size;
	}

	unsigned MemoryStream::GetChecksum()
	{
		if (checksum_)
			return checksum_;

		if (!buffer_ || mode_ == FILE_WRITE)
			return 0;

		UNIQUE_PROFILE(CalculateFileChecksum);

		unsigned oldPos = position_;
		checksum_ = 0;

		Seek(0);
		while (!IsEof())
		{
			unsigned char block[1024];
			unsigned readBytes = Read(block, 1024);

			for (unsigned i = 0; i < readBytes; ++i)
				checksum_ = SDBMHash(checksum_, block[i]);
		}

		Seek(oldPos);
		return checksum_;
	}

}
