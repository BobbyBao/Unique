#pragma once
#include "../serialize/SerializeDefs.h"
#include "../core/SpinLock.h"
#include "../Core/StringUtils.h"
#include "../Container/Str.h"

namespace Unique
{

	/**
	 * A string identifier that provides very fast comparisons to other string identifiers. Significantly faster than
	 * comparing raw strings.
	 *
	 * @note	
	 * Essentially a unique ID is generated for each string and then the ID is used for comparisons as if you were using 
	 * an integer or an enum.
	 * @note
	 * Thread safe.
	 */
	class UNIQUE_API StringID
	{
		static const int HASH_TABLE_SIZE = 4096;
		static const int MAX_CHUNK_COUNT = 50;
		static const int ELEMENTS_PER_CHUNK = 256;
		static const int STRING_SIZE = 256;

		/** Helper class that performs string actions on both null terminated character arrays and standard strings. */
		template<class T>
		class StringIDUtil
		{
		public:
			static uint size(T const& input) { return 0; }
			static void copy(T const& input, char* dest) { }
			static bool compare(T const& a, char* b) { return 0; }
		};

		/**	Internal data that is shared by all instances for a specific string. */
		struct InternalData
		{
			InternalData* next;
			char chars[1];
		};

		/**	Performs initialization of static members as soon as the library is loaded. */
		struct InitStatics
		{
			InitStatics();
		};

	public:
		StringID();

		StringID(const char* name)
			:internalData_(nullptr)
		{
			construct(name);
		}

		StringID(const String& name)
			:internalData_(nullptr)
		{
			construct(name);
		}

		template<int N>
		StringID(const char name[N])
			:internalData_(nullptr)
		{
			construct((const char*)name);
		}

		/**	Compare to string ids for equality. Uses fast integer comparison. */
		bool operator== (const StringID& rhs) const
		{
			return internalData_ == rhs.internalData_;
		}

		/**	Compare to string ids for inequality. Uses fast integer comparison. */
		bool operator!= (const StringID& rhs) const
		{
			return internalData_ != rhs.internalData_;
		}

		/**	Returns true if the string id has no value assigned. */
		bool empty() const
		{
			return internalData_ == nullptr;
		}

		/**	Returns the null-terminated name of the string id. */
		const char* c_str() const
		{
			if (internalData_ == nullptr)
				return nullptr;

			return internalData_->chars;
		}

		String ToString() const {
			return internalData_ ? internalData_->chars : String::EMPTY;
		}

		size_t ToHash() const { return internalData_ ? (size_t)internalData_->chars : 0; }
		
		inline void* GetInternalData() { return internalData_; }

		static const StringID NONE;


	private:
		/**Constructs a StringID object in a way that works for pointers to character arrays and standard strings. */
		template<class T>
		void construct(T const& name);

		/**	Calculates a hash value for the provided null-terminated string. */
		template<class T>
		uint calcHash(T const& input, uint& size);

		/**
		 * Allocates a new string entry and assigns it a unique ID. Optionally expands the chunks buffer if the new entry 
		 * doesn't fit.
		 */
		InternalData* allocEntry(uint size);

		InternalData* internalData_;

		static volatile InitStatics mInitStatics;
		static InternalData* mStringHashTable[HASH_TABLE_SIZE];
		static InternalData* mChunks[MAX_CHUNK_COUNT];
		static uint nextOffset_;
		static uint currentChunk_;
		static uint mNumChunks;
		static SpinLock mSync;
	};

	template<>
	class SerializeTraits<Unique::StringID> : public SerializeTraitsPrimitive<Unique::StringID>
	{
	public:
		template<class TransferFunction>
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			if (transfer.IsReading())
			{
				String temp;
				transfer.TransferPrimitive(temp);
				data = StringID(temp);
			}
			else
			{
				String temp = data.ToString();
				transfer.TransferPrimitive(temp);
			}

		}
	};
}

namespace std
{
/**	Hash value generator for StringID. */
template<>
struct hash<Unique::StringID>
{
	size_t operator()(const Unique::StringID& value) const
	{
		return value.ToHash();
	}
};	

template<>
struct hash<std::pair<Unique::StringID, Unique::StringID> >
{
	size_t operator()(const std::pair<Unique::StringID, Unique::StringID>& _Keyval) const
	{
		return _Keyval.first.ToHash() ^ _Keyval.second.ToHash();
	}
};
}

/** @} */
/** @endcond */