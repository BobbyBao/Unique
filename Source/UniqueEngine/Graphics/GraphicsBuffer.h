#pragma once
#include "Core/Object.h"
#include <Buffer.h>
#include "GPUObject.h"

namespace Unique
{
	class UNIQUE_API GraphicsBuffer : public GPUObject<Object, IBuffer>
	{
		uRTTI(GraphicsBuffer, Object)
	public:
		GraphicsBuffer(uint flags = BIND_NONE);
		~GraphicsBuffer();

		template<class T>
		bool Create(const Vector<T>& data, Usage usage = Usage::USAGE_STATIC)
		{
			return Create((uint)data.size(), sizeof(T), usage, (void*)data.data());
		}

		template<class T>
		bool Create(const T& data, Usage usage = Usage::USAGE_STATIC)
		{
			return Create(1, sizeof(T), usage, (void*)&data);
		}

		/// Set size, vertex elements and dynamic mode. Previous data will be lost.
		bool Create(uint elementCount, uint elementSize, Usage usage = Usage::USAGE_STATIC, void* data = 0);
		bool Create(ByteArray&& data, uint elementSize, Usage usage = Usage::USAGE_STATIC);

		bool SetData(const void* data);
		/// Set a data range in the buffer. Optionally discard data outside the range.
		bool SetDataRange(const void* data, unsigned start, unsigned count, bool discard = false);

		inline bool IsDynamic() const { return desc_.Usage == USAGE::USAGE_DYNAMIC; }
		inline uint GetSizeInBytes() const { return desc_.uiSizeInBytes; }
		inline uint GetCount() const { return desc_.uiSizeInBytes / desc_.ElementByteStride; }
		inline uint GetStride() const { return desc_.ElementByteStride; }

		virtual void UpdateBuffer();
	protected:
		/// Create buffer.
		virtual bool CreateImpl();

		BufferDesc desc_;
		ByteArray data_;
		ByteArray data1_;
	};

	class UNIQUE_API VertexBuffer : public GraphicsBuffer
	{
		uRTTI(VertexBuffer, GraphicsBuffer)

	public:
		VertexBuffer() : GraphicsBuffer(BIND_VERTEX_BUFFER) {}

		bool CreateByMask(uint elementMask, ByteArray&& data, Usage usage = Usage::USAGE_STATIC);

		/*
		/// Return vertex element, or null if does not exist.
		const LayoutElement* GetElement(VertexElementSemantic semantic, unsigned char index = 0) const;

		/// Return vertex element with specific type, or null if does not exist.
		const LayoutElement* GetElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0) const;

		/// Return whether has a specified element semantic.
		bool HasElement(VertexElementSemantic semantic, unsigned char index = 0) const { return GetElement(semantic, index) != 0; }

		/// Return whether has an element semantic with specific type.
		bool HasElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0) const { return GetElement(type, semantic, index) != 0; }

		/// Return offset of a element within vertex, or M_MAX_UNSIGNED if does not exist.
		unsigned GetElementOffset(VertexElementSemantic semantic, unsigned char index = 0) const { const LayoutElement* element = GetElement(semantic, index); return element ? element->offset_ : M_MAX_UNSIGNED; }

		/// Return offset of a element with specific type within vertex, or M_MAX_UNSIGNED if element does not exist.
		unsigned GetElementOffset(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0) const { const LayoutElement* element = GetElement(type, semantic, index); return element ? element->offset_ : M_MAX_UNSIGNED; }
		*/
		/// Return legacy vertex element mask. Note that both semantic and type must match the legacy element for a mask bit to be set.
		unsigned GetElementMask() const { return elementMask_; }

		/// Return buffer hash for building vertex declarations. Used internally.
		unsigned long long GetBufferHash(unsigned streamIndex) { return elementHash_ << (streamIndex * 16); }

		/// Return element with specified type and semantic from a vertex element list, or null if does not exist.
//		static const LayoutElement* GetElement(const PODVector<LayoutElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0);

		/// Return whether element list has a specified element type and semantic.
//		static bool HasElement(const PODVector<LayoutElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0);

		/// Return element offset for specified type and semantic from a vertex element list, or M_MAX_UNSIGNED if does not exist.
//		static unsigned GetElementOffset(const PODVector<LayoutElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0);

		static Vector<LayoutElement> GetElements(uint elementMask);

		/// Return vertex size from an element list.
		static unsigned GetVertexSize(const PODVector<LayoutElement>& elements);

		/// Return vertex size for a legacy vertex element bitmask.
		static unsigned GetVertexSize(unsigned elementMask);

		/// Update offsets of vertex elements.
		static void UpdateOffsets(PODVector<LayoutElement>& elements);
	private:
		/// Update offsets of vertex elements.
		void UpdateOffsets();
		/// Vertex elements.
		PODVector<LayoutElement> elements_;
		/// Vertex element hash.
		unsigned long long elementHash_;
		/// Vertex element legacy bitmask.
		unsigned elementMask_;
	};

	class UNIQUE_API IndexBuffer : public GraphicsBuffer
	{
		uRTTI(IndexBuffer, GraphicsBuffer)
	public:
		IndexBuffer() : GraphicsBuffer(BIND_INDEX_BUFFER) {}
	};

	class UNIQUE_API ConsterBuffer : public GraphicsBuffer
	{
		uRTTI(ConsterBuffer, GraphicsBuffer)
	public:
		ConsterBuffer() : GraphicsBuffer(BIND_UNIFORM_BUFFER) {}
	};
}
