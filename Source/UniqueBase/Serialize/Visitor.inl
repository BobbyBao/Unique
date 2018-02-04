
namespace Unique
{
	template<class T>
	inline void Visitor::VisitAttribute(const char* name, T& data, AttributeFlag metaFlag)
	{
		AttributeFlag metaFlagSave = attributeFlag_;

		attributeFlag_ = metaFlag;

		if (TypeTraits<T>::IsArray())
		{
			attributeFlag_ |= AF_VECTOR;
		}

		if (TypeTraits<T>::IsMap())
		{
			attributeFlag_ |= AF_MAP;
		}

		if (StartAttribute(name))
		{
			Unique::TypeTraits<T>::Visit(data, *this);
			EndAttribute();
		}

		attributeFlag_ = metaFlagSave;
	}

	template <typename... Rest>
	inline void Visitor::VisitAttributes(Rest&... rest)
	{
		int sz = sizeof ...(Rest);
		StartObject(sz / 2);
		VisitImpl1(rest...);
		EndObject();
	}

	template<class T>
	inline void Visitor::Visit(T& data)
	{
		if (Visit(TypeInfo::GetTypeID<T>(), &data))
		{
			return;
		}

		data.Visit(*this);
	}

	template<class T>
	inline void Visitor::VisitObject(SPtr<T>& data)
	{
		if (IsReading())
		{
			if (data == nullptr)
			{
				data = StaticCast<T, Object>(CreateObject());
			}

		}

		if (data)
		{
			data->Visit(*this);
		}

	}

	template<class T>
	inline void Visitor::VisitArray(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		uint size = 0;
		if (IsReading())
		{
			data.clear();
			if (StartArray(size))
			{
				for (uint i = 0; i < size; ++i)
				{
					SetElement(i);
					non_const_value_type val;
					TypeTraits<non_const_value_type>::Visit(val, *this);
					data.push_back(val);
				}

				EndArray();
			}

		}
		else
		{
			size = (uint)data.size();
			if (StartArray(size))
			{
				for (non_const_value_type& val : data)
				{
					TypeTraits<non_const_value_type>::Visit(val, *this);
				}

				EndArray();
			}

		}

	}

	template<class T>
	inline void Visitor::VisitMap(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
		typedef typename non_const_value_type::first_type first_type;
		typedef typename non_const_value_type::second_type second_type;

		uint size = 0;
		if (IsReading())
		{
			data.clear();
			if (StartArray(size))
			{
				for (uint i = 0; i < size; ++i)
				{
					SetElement(i);
					non_const_value_type val;
					TypeTraits<non_const_value_type>::Visit(val, *this);
					data.insert(val);
				}

				EndArray();
			}

		}
		else
		{
			size = (uint)data.size();
			if (StartArray(size))
			{
				for (non_const_value_type& val : data)
				{
					TypeTraits<non_const_value_type>::Visit(val, *this);
				}

				EndArray();
			}

		}
	}


	template<class T>
	inline void Visitor::VisitSet(T& data, int metaFlag)
	{
		typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

		uint size = 0;
		if (IsReading())
		{
			data.clear();
			if (StartArray(size))
			{
				for (uint i = 0; i < size; ++i)
				{
					SetElement(i);
					non_const_value_type val;
					TypeTraits<non_const_value_type>::Visit(val, *this);
					data.insert(val);
				}

				EndArray();
			}

		}
		else
		{
			size = (uint)data.size();
			if (StartArray(size))
			{
				for (non_const_value_type& val : data)
				{
					TypeTraits<non_const_value_type>::Visit(val, *this);
				}

				EndArray();
			}

		}
	}

}