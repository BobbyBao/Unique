#pragma once

namespace Unique
{

	static const int GUID_STRING_LEN = 32;

	struct UNIQUE_API Guid
	{
		uint data[4];
		Guid(uint a, uint b, uint c, uint d) { data[0] = a; data[1] = b; data[2] = c; data[3] = d; }
		Guid() { data[0] = 0; data[1] = 0; data[2] = 0; data[3] = 0; }

		bool operator == (const Guid& rhs) const
		{
			return data[0] == rhs.data[0] && data[1] == rhs.data[1] && data[2] == rhs.data[2] && data[3] == rhs.data[3];
		}

		bool operator != (const Guid& rhs) const { return !(*this == rhs); }

		bool operator < (const Guid& rhs) const { return CompareGUID(*this, rhs) == -1; }
	
		friend int CompareGUID(const Guid& lhs, const Guid& rhs);

		// Use this instead of guid != UnityGUID() -- Will often result in self-documented code
		bool IsValid() const { return data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0; }

		void Init();

		size_t toHash() const
		{
			return 0;
		//	return std::_Hash_seq((byte*)data, 16);
		}

		String ToString();
	};

	void GUIDToString(const Guid& guid, char* string);

	Guid StringToGUID(const String& guidString);
	Guid StringToGUID(const char* guidString, size_t stringLength);

	inline int CompareGUID(const Guid& lhs, const Guid& rhs)
	{
		for (int i = 0; i < 4; i++)
		{
			if (lhs.data[i] < rhs.data[i])
				return -1;
			if (lhs.data[i] > rhs.data[i])
				return 1;
		}
		return 0;
	}

	bool CompareGUIDStringLess(const Guid& lhs, const Guid& rhs);

}

namespace std
{
	template<>
	struct hash<Unique::Guid>
	{
		size_t operator()(const Unique::Guid& _Keyval) const
		{
			return _Keyval.toHash();
		}
	};
}