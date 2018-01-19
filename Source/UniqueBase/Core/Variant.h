#pragma once

namespace Unique
{
	/// Variant's supported types.
	enum VariantType : uint
	{
		VAR_NONE = 0,
		VAR_INT,
		VAR_BOOL,
		VAR_FLOAT,
		VAR_INT64,
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
		ValueType operator()() const
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

}