#include "Precompiled.h"
#include "GraphicsBuffer.h"
#include "Graphics/Graphics.h"
#include <InputLayout.h>

namespace Unique
{
	uObject(GraphicsBuffer)
	{
	}

	uObject(VertexBuffer)
	{
		uFactory("Graphics")
			uAttribute("Data", data_)
	}

	uObject(IndexBuffer)
	{
		uFactory("Graphics")
			uAttribute("Data", data_)
	}

	uObject(ConsterBuffer)
	{
		uFactory("Graphics")
			uAttribute("Data", data_)
	}

	GraphicsBuffer::GraphicsBuffer(uint flags)
	{
		desc_.BindFlags = flags;
	}

	GraphicsBuffer::~GraphicsBuffer()
	{
	}

	bool GraphicsBuffer::Create(uint elementCount, uint elementSize, Usage usage, void* data)
	{
		desc_.ElementByteStride = elementSize;
		desc_.uiSizeInBytes = elementCount*elementSize;
		desc_.Usage = usage;

		data_.resize(desc_.uiSizeInBytes);
		if (data)
		{
			std::memcpy(data_.data(), data, desc_.uiSizeInBytes);
		}

		return GPUObject<Object, IBuffer>::Create();
	}

	bool GraphicsBuffer::Create(ByteArray&& data, uint elementSize, Usage usage)
	{
		desc_.ElementByteStride = elementSize;
		desc_.uiSizeInBytes = (uint)data.size();
		desc_.Usage = usage;
		data_ = data;

		return GPUObject<Object, IBuffer>::Create();
	}

	bool GraphicsBuffer::CreateImpl()
	{
		if (!GetSizeInBytes())
			return false;

		BufferData BuffData;
		BuffData.pData = data_.data();
		BuffData.DataSize = desc_.uiSizeInBytes;
		renderDevice->CreateBuffer(desc_, BuffData, &handle_);
		return handle_ != nullptr;
	}

	bool GraphicsBuffer::SetData(const void* data)
	{
		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for vertex buffer data");
			return false;
		}

		if (GetSizeInBytes() == 0)
		{
			UNIQUE_LOGERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if (IsDynamic())
		{
			ByteArray& currentData = Graphics::currentContext_ == 0 ? data_ : data1_;
			std::memcpy(currentData.data(), data, currentData.size());
		}
		else
		{
			std::memcpy(data_.data(), data, data_.size());
			/*
			GraphicsContext::AddCommand([=]()
			{
				void* buffer = renderer->MapBuffer(*this, LLGL::BufferCPUAccess::WriteOnly);
				memcpy(buffer, data_.data(), data_.size());
				renderer->UnmapBuffer(*this);
			});*/
		}


		return true;
	}

	bool GraphicsBuffer::SetDataRange(const void* data, unsigned start, unsigned count, bool discard)
	{
		if (start == 0 && count == GetCount())
			return SetData(data);

		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for vertex buffer data");
			return false;
		}

		if (GetStride() == 0)
		{
			UNIQUE_LOGERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if (start + count > GetCount())
		{
			UNIQUE_LOGERROR("Illegal range for setting new vertex buffer data");
			return false;
		}

		if (!count)
			return true;

		if (IsDynamic())
		{
			ByteArray& currentData = Graphics::currentContext_ == 0 ? data_ : data1_;
			std::memcpy(currentData.data() + start * GetStride(), data, count * GetStride());
		}
		else
		{
			std::memcpy(data_.data() + start * GetStride(), data, count * GetStride());
			/*
			GraphicsContext::AddCommand([=]()
			{
				void* buffer = renderer->MapBuffer(*this, LLGL::BufferCPUAccess::WriteOnly);
				memcpy(buffer, data_.data() + start * elementSize_, count * elementSize_);
				renderer->UnmapBuffer(*this);
			});*/
		}

		return true;
	}

	void GraphicsBuffer::UpdateBuffer()
	{
	}


	extern UNIQUE_API const LayoutElement LEGACY_VERTEXELEMENTS[] =
	{
		LayoutElement(0, 0, 3, VT_FLOAT32, False),     // Position
		LayoutElement(0, 0, 3, VT_FLOAT32, False),       // Normal
		LayoutElement(0, 0, 4, VT_UINT8, True),    // Color
		LayoutElement(0, 0, 2, VT_FLOAT32, False),     // Texcoord1
		LayoutElement(0, 0, 2, VT_FLOAT32, False),     // Texcoord2
		LayoutElement(0, 0, 3, VT_FLOAT32, False),     // Cubetexcoord1
		LayoutElement(0, 0, 3, VT_FLOAT32, False),     // Cubetexcoord2
		LayoutElement(0, 0, 4, VT_FLOAT32, False),      // Tangent
		LayoutElement(0, 0, 4, VT_FLOAT32, False), // Blendweights
		LayoutElement(0, 0, 4, VT_UINT8, False),  // Blendindices
		LayoutElement(0, 0, 4, VT_FLOAT32, False, 0, LayoutElement::FREQUENCY_PER_INSTANCE),      // Instancematrix1
		LayoutElement(0, 0, 4, VT_FLOAT32, False, 0, LayoutElement::FREQUENCY_PER_INSTANCE),      // Instancematrix2
		LayoutElement(0, 0, 4, VT_FLOAT32, False, 0, LayoutElement::FREQUENCY_PER_INSTANCE),      // Instancematrix3
		LayoutElement(0, 0, 1, VT_UINT32, False)       // Objectindex
	};

	extern UNIQUE_API const unsigned ELEMENT_TYPESIZES[] =
	{
		0,
		sizeof(char),
		sizeof(short),
		sizeof(int),
		sizeof(unsigned char),
		sizeof(unsigned short),
		sizeof(unsigned int),
		sizeof(unsigned short),
		sizeof(float)
	};


	bool VertexBuffer::CreateByMask(uint elementMask, ByteArray&& data, Usage usage)
	{
		elementMask_ = elementMask;
		elements_ = GetElements(elementMask);

		desc_.ElementByteStride = GetVertexSize(elements_);
		desc_.uiSizeInBytes = (uint)data.size();
		desc_.Usage = usage;
		data_ = data;

		UpdateOffsets(elements_);
		return GPUObject::Create();
	}
	/*
	const LayoutElement* VertexBuffer::GetElement(VertexElementSemantic semantic, unsigned char index) const
	{
		for (PODVector<LayoutElement>::const_iterator i = elements_.begin(); i != elements_.end(); ++i)
		{
			if (i->semantic_ == semantic && i->index_ == index)
				return &(*i);
		}

		return 0;
	}

	const LayoutElement* VertexBuffer::GetElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index) const
	{
		for (PODVector<LayoutElement>::const_iterator i = elements_.begin(); i != elements_.end(); ++i)
		{
			if (i->type_ == type && i->semantic_ == semantic && i->index_ == index)
				return &(*i);
		}

		return 0;
	}

	const LayoutElement* VertexBuffer::GetElement(const PODVector<LayoutElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index)
	{
		for (PODVector<LayoutElement>::const_iterator i = elements.begin(); i != elements.end(); ++i)
		{
			if (i->type_ == type && i->semantic_ == semantic && i->index_ == index)
				return &(*i);
		}

		return 0;
	}

	bool VertexBuffer::HasElement(const PODVector<LayoutElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index)
	{
		return GetElement(elements, type, semantic, index) != 0;
	}

	unsigned VertexBuffer::GetElementOffset(const PODVector<LayoutElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index)
	{
		const LayoutElement* element = GetElement(elements, type, semantic, index);
		return element ? element->offset_ : M_MAX_UNSIGNED;
	}*/

	Vector<LayoutElement> VertexBuffer::GetElements(uint elementMask)
	{
		Vector<LayoutElement> ret;
		for (unsigned i = 0; i < MAX_LEGACY_VERTEX_ELEMENTS; ++i)
		{
			if (elementMask & (1 << i))
				ret.push_back(LEGACY_VERTEXELEMENTS[i]);
		}
		return ret;
	}

	unsigned VertexBuffer::GetVertexSize(const PODVector<LayoutElement>& elements)
	{
		unsigned size = 0;

		for (unsigned i = 0; i < elements.size(); ++i)
		{
			const LayoutElement& layoutEle = elements[i];
			size += (layoutEle.NumComponents * ELEMENT_TYPESIZES[layoutEle.ValueType]);
		}

		return size;
	}

	unsigned VertexBuffer::GetVertexSize(unsigned elementMask)
	{
		unsigned size = 0;

		for (unsigned i = 0; i < MAX_LEGACY_VERTEX_ELEMENTS; ++i)
		{
			if (elementMask & (1 << i))
			{
				const LayoutElement& layoutEle = LEGACY_VERTEXELEMENTS[i];
				size += (layoutEle.NumComponents * ELEMENT_TYPESIZES[layoutEle.ValueType]);
			}
		}

		return size;
	}

	void VertexBuffer::UpdateOffsets(PODVector<LayoutElement>& elements)
	{
		unsigned elementOffset = 0;

		for (auto i = elements.begin(); i != elements.end(); ++i)
		{
			const LayoutElement& layoutEle = *i;
			i->RelativeOffset = elementOffset;
			elementOffset += (layoutEle.NumComponents * ELEMENT_TYPESIZES[layoutEle.ValueType]);
		}
	}
}
