#include "Precompiled.h"
#include "VertexBuffer.h"

namespace Unique
{
	uObject(VertexBuffer)
	{
		uFactory("Graphics")
			uAttribute("Data", data_)
	}


	extern UNIQUE_API const VertexElement LEGACY_VERTEXELEMENTS[] =
	{
		VertexElement(TYPE_VECTOR3, SEM_POSITION, 0, false),     // Position
		VertexElement(TYPE_VECTOR3, SEM_NORMAL, 0, false),       // Normal
		VertexElement(TYPE_UBYTE4_NORM, SEM_COLOR, 0, false),    // Color
		VertexElement(TYPE_VECTOR2, SEM_TEXCOORD, 0, false),     // Texcoord1
		VertexElement(TYPE_VECTOR2, SEM_TEXCOORD, 1, false),     // Texcoord2
		VertexElement(TYPE_VECTOR3, SEM_TEXCOORD, 0, false),     // Cubetexcoord1
		VertexElement(TYPE_VECTOR3, SEM_TEXCOORD, 1, false),     // Cubetexcoord2
		VertexElement(TYPE_VECTOR4, SEM_TANGENT, 0, false),      // Tangent
		VertexElement(TYPE_VECTOR4, SEM_BLENDWEIGHTS, 0, false), // Blendweights
		VertexElement(TYPE_UBYTE4, SEM_BLENDINDICES, 0, false),  // Blendindices
		VertexElement(TYPE_VECTOR4, SEM_TEXCOORD, 4, true),      // Instancematrix1
		VertexElement(TYPE_VECTOR4, SEM_TEXCOORD, 5, true),      // Instancematrix2
		VertexElement(TYPE_VECTOR4, SEM_TEXCOORD, 6, true),      // Instancematrix3
		VertexElement(TYPE_INT, SEM_OBJECTINDEX, 0, false)       // Objectindex
	};

	extern UNIQUE_API const unsigned ELEMENT_TYPESIZES[] =
	{
		sizeof(int),
		sizeof(float),
		2 * sizeof(float),
		3 * sizeof(float),
		4 * sizeof(float),
		sizeof(unsigned),
		sizeof(unsigned)
	};

	bool VertexBuffer::CreateByMask(uint elementMask, ByteArray&& data, Usage usage)
	{
		elementMask_ = elementMask;
		elements_ = GetElements(elementMask);
		UpdateOffsets();

		desc_.ElementByteStride = GetVertexSize(elements_);
		desc_.uiSizeInBytes = data.size();
		desc_.Usage = usage;
		data_ = data;

		return GPUObject::Create();
	}

	void VertexBuffer::UpdateOffsets()
	{
		unsigned elementOffset = 0;
		elementHash_ = 0;
		elementMask_ = 0;

		for (auto i = elements_.begin(); i != elements_.end(); ++i)
		{
			i->offset_ = elementOffset;
			elementOffset += ELEMENT_TYPESIZES[i->type_];
			elementHash_ <<= 6;
			elementHash_ += (((int)i->type_ + 1) * ((int)i->semantic_ + 1) + i->index_);

			for (unsigned j = 0; j < MAX_LEGACY_VERTEX_ELEMENTS; ++j)
			{
				const VertexElement& legacy = LEGACY_VERTEXELEMENTS[j];
				if (i->type_ == legacy.type_ && i->semantic_ == legacy.semantic_ && i->index_ == legacy.index_)
					elementMask_ |= (1 << j);
			}
		}

		desc_.ElementByteStride = elementOffset;
	}

	const VertexElement* VertexBuffer::GetElement(VertexElementSemantic semantic, unsigned char index) const
	{
		for (auto i = elements_.begin(); i != elements_.end(); ++i)
		{
			if (i->semantic_ == semantic && i->index_ == index)
				return &(*i);
		}

		return nullptr;
	}

	const VertexElement* VertexBuffer::GetElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index) const
	{
		for (auto i = elements_.begin(); i != elements_.end(); ++i)
		{
			if (i->type_ == type && i->semantic_ == semantic && i->index_ == index)
				return &(*i);
		}

		return nullptr;
	}

	const VertexElement* VertexBuffer::GetElement(const PODVector<VertexElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index)
	{
		for (auto i = elements.begin(); i != elements.end(); ++i)
		{
			if (i->type_ == type && i->semantic_ == semantic && i->index_ == index)
				return &(*i);
		}

		return nullptr;
	}

	bool VertexBuffer::HasElement(const PODVector<VertexElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index)
	{
		return GetElement(elements, type, semantic, index) != nullptr;
	}

	unsigned VertexBuffer::GetElementOffset(const PODVector<VertexElement>& elements, VertexElementType type, VertexElementSemantic semantic, unsigned char index)
	{
		const VertexElement* element = GetElement(elements, type, semantic, index);
		return element ? element->offset_ : M_MAX_UNSIGNED;
	}

	PODVector<VertexElement> VertexBuffer::GetElements(unsigned elementMask)
	{
		PODVector<VertexElement> ret;

		for (unsigned i = 0; i < MAX_LEGACY_VERTEX_ELEMENTS; ++i)
		{
			if (elementMask & (1 << i))
				ret.push_back(LEGACY_VERTEXELEMENTS[i]);
		}

		return ret;
	}

	unsigned VertexBuffer::GetVertexSize(const PODVector<VertexElement>& elements)
	{
		unsigned size = 0;

		for (size_t i = 0; i < elements.size(); ++i)
			size += ELEMENT_TYPESIZES[elements[i].type_];

		return size;
	}

	unsigned VertexBuffer::GetVertexSize(unsigned elementMask)
	{
		unsigned size = 0;

		for (unsigned i = 0; i < MAX_LEGACY_VERTEX_ELEMENTS; ++i)
		{
			if (elementMask & (1 << i))
				size += ELEMENT_TYPESIZES[LEGACY_VERTEXELEMENTS[i].type_];
		}

		return size;
	}

	void VertexBuffer::UpdateOffsets(PODVector<VertexElement>& elements)
	{
		unsigned elementOffset = 0;

		for (auto i = elements.begin(); i != elements.end(); ++i)
		{
			i->offset_ = elementOffset;
			elementOffset += ELEMENT_TYPESIZES[i->type_];
		}
	}

}

