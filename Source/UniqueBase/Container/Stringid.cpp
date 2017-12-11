#include "Precompiled.h"
#include "StringID.h"


namespace Unique
{
	const StringID StringID::NONE = StringID();

	const StringID StringID::EMPTY = StringID("");

	volatile StringID::InitStatics StringID::mInitStatics = StringID::InitStatics();

	StringID::InternalData* StringID::mStringHashTable[HASH_TABLE_SIZE];
	StringID::InternalData* StringID::mChunks[MAX_CHUNK_COUNT];

	uint StringID::nextOffset_ = 0;
	uint StringID::currentChunk_ = -1;
	uint StringID::mNumChunks = 0;
	SpinLock StringID::mSync;
	bool inited = false;
	StringID::InitStatics::InitStatics()
	{
		ScopedSpinLock lock(mSync);

		if (inited)
		{
			return;
		}

		memset(mStringHashTable, 0, sizeof(mStringHashTable));
		memset(mChunks, 0, sizeof(mChunks));
		uint chunkSize = ELEMENTS_PER_CHUNK * 256;
		currentChunk_ = 0;
		mChunks[currentChunk_] = (InternalData*)malloc(chunkSize);
		memset(mChunks[currentChunk_], 0, chunkSize);
		mNumChunks++;
		inited = true;
	}

	StringID::StringID()
		:internalData_(nullptr)
	{ }

	template<class T>
	void StringID::construct(T const& name)
	{
		if (!inited)
		{
			InitStatics();
		}

		assert(StringIDUtil<T>::size(name) <= STRING_SIZE);

		uint size;
		uint hash = calcHash(name, size) & (sizeof(mStringHashTable) / sizeof(mStringHashTable[0]) - 1);
		InternalData* existingEntry = mStringHashTable[hash];
		
		while (existingEntry != nullptr)
		{
			if (StringIDUtil<T>::compare(name, existingEntry->chars))
			{
				internalData_ = existingEntry;
				return;
			}

			existingEntry = existingEntry->next;
		}

		ScopedSpinLock lock(mSync);

		// Search for the value again in case other thread just added it
		existingEntry = mStringHashTable[hash];
		InternalData* lastEntry = nullptr;
		while (existingEntry != nullptr)
		{
			if (StringIDUtil<T>::compare(name, existingEntry->chars))
			{
				internalData_ = existingEntry;
				return;
			}

			lastEntry = existingEntry;
			existingEntry = existingEntry->next;
		}

		internalData_ = allocEntry(size);
		StringIDUtil<T>::copy(name, internalData_->chars);

		if (lastEntry == nullptr)
			mStringHashTable[hash] = internalData_;
		else
			lastEntry->next = internalData_;
	}

	template<class T>
	uint StringID::calcHash(T const& input, uint& size)
	{
		size = StringIDUtil<T>::size(input);
		
		uint hash = 0;
		for (uint i = 0; i < size; i++)
			hash = hash * 101 + std::tolower(input[i]);

		return hash;
	}

	StringID::InternalData* StringID::allocEntry(uint size)
	{
		uint chunkSize = ELEMENTS_PER_CHUNK * 256;
		int headSize = sizeof(InternalData*) + 1;
		if ((size + headSize + nextOffset_) * sizeof(char) > chunkSize)
		{
			assert(currentChunk_ < MAX_CHUNK_COUNT);

			currentChunk_++;
			mChunks[currentChunk_] = (InternalData*)malloc(chunkSize);
			memset(mChunks[currentChunk_], 0, chunkSize);
			nextOffset_ = 0;
			mNumChunks++;
		}

		InternalData* chunk = mChunks[currentChunk_];
		InternalData* newEntry = (InternalData*)&((byte*)chunk)[nextOffset_];

		newEntry->next = nullptr;
		nextOffset_ += (size + headSize);

		return newEntry;
	}

	template<>
	class StringID::StringIDUtil<const char*>
	{
	public:
		static uint size(const char* const& input) { return (uint)strlen(input); }
		static void copy(const char* const& input, char* dest) { memcpy(dest, input, strlen(input) + 1); }
		static bool compare(const char* const& a, char* b) { return _stricmp(a, b) == 0; }
	};

	template<>
	class StringID::StringIDUtil <String>
	{
	public:
		static uint size(String const& input) { return (uint)input.Length(); }
		static void copy(String const& input, char* dest)
		{ 
			uint len = (uint)input.Length();
			std::strcpy(dest, input.CString());
			dest[len] = '\0';
		}
		static bool compare(String const& a, char* b) { return _stricmp(a.CString(), b) == 0; }
	};

	template class StringID::StringIDUtil <const char*>;
	template class StringID::StringIDUtil <String>;

	template UNIQUE_API void StringID::construct(const char* const&);
	template UNIQUE_API void StringID::construct(String const&);
	
	template UNIQUE_API uint StringID::calcHash(const char* const&, uint& size);
	template UNIQUE_API uint StringID::calcHash(String const&, uint& size);
}