#pragma once

namespace Unique
{
	/// Variant's supported types.
	enum VariantType : uint
	{
		VAR_NONE = 0,
		VAR_BOOL,
		VAR_CHAR,
		VAR_UCHAR,
		VAR_SHORT,
		VAR_USHORT,
		VAR_INT,
		VAR_UINT,
		VAR_INT64,
		VAR_UINT64,
		VAR_FLOAT,
		VAR_DOUBLE,
		VAR_VECTOR2,
		VAR_VECTOR3,
		VAR_VECTOR4,
		VAR_QUATERNION,
		VAR_COLOR,
		VAR_STRING,
		VAR_BUFFER,
		VAR_PTR,
		VAR_CUSTOM,
		MAX_VAR_TYPES
	};

	template<class T> struct Type2Id { const uint Id = VAR_CUSTOM;};
	
	class UNIQUE_API Variant 
	{
	public:
		Variant() : mType(VAR_NONE)
		{
		}

		template<typename ValueType>
		explicit Variant(const ValueType & value)
		{
			mType = Type2Id<ValueType>::Id;

			if (sizeof(ValueType) < sizeof(mData))
			{
				new(mData) ValueType(value);
			}
			else
			{
				mContent = new holder<ValueType>(value);
				mType = VAR_CUSTOM;
			}

		}

		Variant(const Variant & other);
		virtual ~Variant();
	public:

		template<typename ValueType>
		Variant& operator=(const ValueType & rhs)
		{
			Variant(rhs).swap(*this);
			return *this;
		}

		Variant & operator=(const Variant & rhs)
		{
			Variant(rhs).swap(*this);
			return *this;

		}

		VariantType getPropertyType() const
		{
			return (VariantType)mType;
		}

		void setPropertyType(VariantType val)
		{
			mType = val;
		}

		bool isEmpty() const
		{
			return mType == VAR_NONE;
		}
		
		bool isCustom() const
		{
			return mType == VAR_CUSTOM;
		}
		
		byte* ptr()
		{
			return mData;
		}

		void destroy();

		Variant& swap(Variant & rhs)
		{
			std::swap(mData, rhs.mData);
			std::swap(mType, rhs.mType);
			return *this;
		}

	protected:

		class placeholder 
		{
		public:

			virtual ~placeholder()
			{
			}

		public:

			virtual const std::type_info& getType() const = 0;
			virtual placeholder * clone() const = 0;
			virtual byte* ptr() = 0;
		};

		template<typename ValueType>
		class holder : public placeholder
		{
		public:

			holder(const ValueType & value)
				: held(value)
			{
			}

		public:

			virtual const std::type_info & getType() const
			{
				return typeid(ValueType);
			}

			virtual placeholder * clone() const
			{
				return new holder(held);
			}

			byte* ptr() { return (byte*)&held; }
		public:

			ValueType held;

		};

	public:
		union
		{
			byte mData[16];
			placeholder * mContent;
		};

		VariantType mType;

		template<typename ValueType>
		friend ValueType * any_cast(Variant *);

		template<typename ValueType>
		operator ValueType()
		{
			if (isEmpty())
			{
				return ValueType();
			}
			else if(isCustom())
			{
				if(getType() == typeid(ValueType))
				{
					return static_cast<Variant::holder<ValueType> *>(mContent)->held;
				}
			}
			else
			{ 
				assert(mType == Type2Id<ValueType>::Id);
				return *(ValueType*)mData;
			}

		}

		static const Variant EMPTY;

	};

	template<> struct Type2Id<bool> { const uint Id = VAR_BOOL; };
	template<> struct Type2Id<char> { const uint Id = VAR_CHAR; };
	template<> struct Type2Id<byte> { const uint Id = VAR_UCHAR; };
	template<> struct Type2Id<short> { const uint Id = VAR_SHORT; };
	template<> struct Type2Id<ushort> { const uint Id = VAR_USHORT; };
	template<> struct Type2Id<int> { const uint Id = VAR_INT; };
	template<> struct Type2Id<uint> { const uint Id = VAR_UINT; };
	template<> struct Type2Id<long> { const uint Id = VAR_INT; };
	template<> struct Type2Id<unsigned long> { const uint Id = VAR_UINT; };
	template<> struct Type2Id<int64> { const uint Id = VAR_INT64; };
	template<> struct Type2Id<uint64> { const uint Id = VAR_UINT64; };
	template<> struct Type2Id<float> { const uint Id = VAR_FLOAT; };
	template<> struct Type2Id<double> { const uint Id = VAR_DOUBLE; };
	template<> struct Type2Id<Vector2> { const uint Id = VAR_VECTOR2; };
	template<> struct Type2Id<Vector3> { const uint Id = VAR_VECTOR3; };
	template<> struct Type2Id<Vector4> { const uint Id = VAR_VECTOR4; };
	template<> struct Type2Id<Quaternion> { const uint Id = VAR_QUATERNION; };
	template<> struct Type2Id<Color> { const uint Id = VAR_COLOR; };
	template<> struct Type2Id<String> { const uint Id = VAR_STRING; };
	template<> struct Type2Id<ByteArray> { const uint Id = VAR_BUFFER; };
	template<> struct Type2Id<void*> { const uint Id = VAR_PTR; };
}