#pragma once
#include "IO/IStream.h"
#include "PrimitiveTypeTraits.h"
#include "StdTypeTraits.h"

namespace Unique
{
	enum VisitState
	{
		VS_READ,
		VS_WRITE,
		VS_READWRITE
	};

	class Object;

	class Visitor
	{
	public:
		Visitor(VisitState state);

		bool IsReading() const { return state_ == VS_READ; }
		bool IsWriting() const { return state_ == VS_WRITE; }

		template<class T>
		void VisitPrimitive(T& data)
		{
			data.Visit(*this);
		}

		template<class T>
		void VisitObject(SPtr<T>& data);

		template<class T>
		void VisitArray(T& data, int metaFlag = 0);

		template<class T>
		void VisitMap(T& data, int metaFlag = 0);

		template<class T>
		void VisitSet(T& data, int metaFlag = 0);

		template<class T>
		void VisitAttribute(const char* name, T& data, AttributeFlag metaFlag = AF_DEFAULT);
		
		template <typename... Rest>
		void VisitAttributes(Rest&... rest);

		template<class T>
		void Visit(T& data);

		bool Visit(const StringID& type, void* obj);

		virtual bool StartDocument(const String& fileName) { return true; }
		virtual void EndDocument() {}
		virtual bool StartObject(uint size) { return true; }
		virtual void EndObject() {}
		virtual SPtr<Object> CreateObject() { return nullptr; }
		virtual bool StartAttribute(const String& key) { return true; }
		virtual void EndAttribute() {}
		virtual bool StartArray(uint& size) { return true; }
		virtual void SetElement(uint index) {}
		virtual void EndArray() {}
		virtual void VisitBin(ByteArray& data) {}
		virtual void VisitPrimitive(std::string& data) {}
		virtual void VisitPrimitive(String& data) {}
		virtual void VisitPrimitive(bool& data) {}
		virtual void VisitPrimitive(char& data) {}
		virtual void VisitPrimitive(unsigned char& data) {}
		virtual void VisitPrimitive(short& data) {}
		virtual void VisitPrimitive(unsigned short& data) {}
		virtual void VisitPrimitive(int& data) {}
		virtual void VisitPrimitive(unsigned int& data) {}
		virtual void VisitPrimitive(long long& data) {}
		virtual void VisitPrimitive(unsigned long long& data) {}
		virtual void VisitPrimitive(float& data) {}
		virtual void VisitPrimitive(double& data) {}
		virtual void VisitPrimitive(Vector2& data) {}
		virtual void VisitPrimitive(Vector3& data) {}
		virtual void VisitPrimitive(Vector4& data) {}
		virtual void VisitPrimitive(Color& data) {}
		virtual void VisitPrimitive(Quaternion& data) {}


	protected:
		template <typename First, typename... Rest>
		void VisitImpl1(First first, Rest&... rest)
		{
			VisitImpl2(first, rest...);
		}

		template <typename First, typename Second, typename... Rest>
		void VisitImpl2(First name, Second& val, Rest&... rest)
		{
			VisitAttribute(name, val);
			VisitImpl1(rest...);
		}

		template <typename First, typename Second, typename... Rest>
		void VisitImpl2(First name, Second& val)
		{
			VisitAttribute(name, val);
		}
		
		VisitState state_;
		AttributeFlag attributeFlag_;

	};

	class Serializer : public Visitor
	{
	public:
		Serializer(VisitState state) : Visitor(state) {}

		template<class T>
		bool Save(const char* fileName, T& data)
		{
			if (!StartDocument(fileName))
			{
				return false;
			}

			TypeTraits<T>::Visit(data, *this);

			EndDocument();

			return true;
		}

		template<class T>
		bool Load(const String& fileName, T& data)
		{
			auto& cache = GetSubsystem<ResourceCache>();
			SPtr<File> file = cache.GetFile(fileName);
			if (!file)
			{
				return false;
			}

			return Load(*file, data);
		}

		template<class T>
		bool Load(IStream& source, T& data)
		{
			data_ = source.ReadAll();

			if (data_.empty())
			{
				return false;
			}

			rootObject_ = TypeTraits<T>::IsObject();
			rootType_ = TypeTraits<T>::GetTypeName();

			StartDocument(source.GetName());
			TypeTraits<T>::Visit(data, *this);
			EndDocument();
			return true;
		}
	protected:
		bool rootObject_ = true;
		std::string rootType_;
		ByteArray data_;
	};

}

#include "Visitor.inl"