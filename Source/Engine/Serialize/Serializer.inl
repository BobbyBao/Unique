
template<class T> inline
bool Serializer::Save(const char* fileName, T& data)
{
	/*
	std::ofstream jsonFile(fileName);
	OStreamWrapper osw(jsonFile);
	PrettyWriter<OStreamWrapper> writer(osw);
	writer_ = &writer;*/

	SerializeTraits<T>::Transfer(data, *this);

	return true;
}

template<class T>
inline void Serializer::Transfer(T& data, const char* name, int metaFlag)
{
	metaFlag_ = metaFlag;

//	writer_->Key(name);

	SerializeTraits<T>::Transfer(data, *this);
}

template<class T>
inline void Serializer::Transfer(T& data)
{
//	writer_->StartObject();

	data.Transfer(*this);

//	writer_->EndObject();
}

template<class T>
inline void Serializer::TransferObject(SPtr<T>& data)
{
//	writer_->StartObject();
	data->VirtualTransfer(*this);
//	writer_->EndObject();
}

template<class T>
inline void Serializer::TransferSTLStyleArray(T& data, int metaFlag)
{
	typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

//	writer_->StartArray();

	for (non_const_value_type& val : data)
	{
		SerializeTraits<non_const_value_type>::Transfer(val, *this);
	}

//	writer_->EndArray();
}

template<class T>
inline void Serializer::TransferSTLStyleMap(T& data, int metaFlag)
{
	typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;
	typedef typename non_const_value_type::first_type first_type;
	typedef typename non_const_value_type::second_type second_type;

//	writer_->StartArray();

	for (non_const_value_type& val : data)
	{
		SerializeTraits<non_const_value_type>::Transfer(val, *this);
	}

//	writer_->EndArray();
}


template<class T>
inline void Serializer::TransferSTLStyleSet(T& data, int metaFlag)
{
	typedef typename NonConstContainerValueType<T>::value_type non_const_value_type;

//	writer_->StartArray();

	for (non_const_value_type& val : data)
	{
		SerializeTraits<non_const_value_type>::Transfer(val, *this);
	}

//	writer_->EndArray();
}

template<class T>
void Serializer::TransferBasicData(T& data)
{
	String str = ToString(data);
//	writer_->String(str.CString());
}

template<>
inline void Serializer::TransferBasicData<String>(String& data)
{
//	writer_->String(data.CString());
}

template<>
inline void Serializer::TransferBasicData<bool>(bool& data)
{
//	writer_->Bool(data);
}

template<>
inline void Serializer::TransferBasicData<char>(char& data)
{
//	writer_->Int(data);
}

template<>
inline void Serializer::TransferBasicData<unsigned char>(unsigned char& data)
{
//	writer_->Uint(data);
}

template<>
inline void Serializer::TransferBasicData<short>(short& data)
{
//	writer_->Int(data);
}

template<>
inline void Serializer::TransferBasicData<unsigned short>(unsigned short& data)
{
//	writer_->Uint(data);
}


template<>
inline void Serializer::TransferBasicData<int>(int& data)
{
//	writer_->Int(data);
}

template<>
inline void Serializer::TransferBasicData<unsigned int>(unsigned int& data)
{
//	writer_->Uint(data);
}

template<>
inline void Serializer::TransferBasicData<long long>(long long& data)
{
//	writer_->Int64(data);
}

template<>
inline void Serializer::TransferBasicData<unsigned long long>(unsigned long long& data)
{
//	writer_->Uint64(data);
}

template<>
inline void Serializer::TransferBasicData<float>(float& data)
{
//	writer_->Double(data);
}

template<>
inline void Serializer::TransferBasicData<double>(double& data)
{
//	writer_->Double(data);
}
