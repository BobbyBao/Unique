#include "Precompiled.h"
#include "../Graphics.h"
#include "VertexBuffer.h"
#include "../../Math/MathDefs.h"
#include <LLGL/Utility.h>
#include "../../DebugNew.h"

namespace Unique
{
	extern UPtr<LLGL::RenderSystem>        renderer;

	const VertexAttribute LEGACY_VERTEXELEMENTS[] =
	{
		VertexAttribute("position", 0, VectorType::Float3, 0),     // Position
		VertexAttribute("normal", 0, VectorType::Float3, 0),       // Normal
		VertexAttribute("color", 0, VectorType::Int4/*TYPE_UBYTE4_NORM*/, 0),    // Color
		VertexAttribute("texcoord", 0, VectorType::Float2, 0),     // Texcoord1
		VertexAttribute("texcoord", 1, VectorType::Float2, 0),     // Texcoord2
		VertexAttribute("texcoord", 0, VectorType::Float3, 0),     // Cubetexcoord1
		VertexAttribute("texcoord", 1, VectorType::Float3, 0),     // Cubetexcoord2
		VertexAttribute("tangent", 0, VectorType::Float4, 0),      // Tangent
		VertexAttribute("blendweights", 0, VectorType::Float4, 0), // Blendweights
		VertexAttribute("blendindices", 0, VectorType::Int4, 0),  // Blendindices
		VertexAttribute("texcoord", 4, VectorType::Float4, true),      // Instancematrix1
		VertexAttribute("texcoord", 5, VectorType::Float4, true),      // Instancematrix2
		VertexAttribute("texcoord", 6, VectorType::Float4, true),      // Instancematrix3
		VertexAttribute("objectindex", 0, VectorType::Int, 0)       // Objectindex
	};

	extern const unsigned ELEMENT_TYPESIZES[] =
	{
		sizeof(int),
		sizeof(float),
		2 * sizeof(float),
		3 * sizeof(float),
		4 * sizeof(float),
		sizeof(unsigned),
		sizeof(unsigned)
	};

	VertexBuffer::VertexBuffer() :
		elementMask_(0)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	bool VertexBuffer::Create(unsigned vertexCount, unsigned elementMask, const ByteArray& data)
	{
		shadowData_ = data;
		vertexFormat_ = GetElements(elementMask);
		handle_ = renderer->CreateBuffer(VertexBufferDesc(vertexCount * vertexFormat_.stride, vertexFormat_), data.data());
		return handle_ != nullptr;
	}

	bool VertexBuffer::SetSize(unsigned vertexCount, unsigned elementMask, long flag)
	{
		return SetSize(vertexCount, GetElements(elementMask), flag);
	}

	bool VertexBuffer::SetSize(unsigned vertexCount, const VertexFormat& elements, long flag)
	{
		Unlock();

		elementCount_ = vertexCount;
		flags_ = flag;
	
		if (shadowed_ && elementCount_ && elementSize_)
			shadowData_.resize(elementCount_ * elementSize_);
		else
			shadowData_.resize(0);

		return Create();
	}
	
	const VertexAttribute* VertexBuffer::GetElement(const std::string& semantic, unsigned char index) const
	{
		for (auto& i = vertexFormat_.attributes.begin(); i != vertexFormat_.attributes.end(); ++i)
		{
			if (i->name == semantic && i->semanticIndex == index)
				return &(*i);
		}

		return 0;
	}

	const VertexAttribute* VertexBuffer::GetElement(VectorType type, const std::string& semantic, unsigned char index) const
	{
		for (auto& i = vertexFormat_.attributes.begin(); i != vertexFormat_.attributes.end(); ++i)
		{
			if (i->vectorType == type && i->name == semantic && i->semanticIndex == index)
				return &(*i);
		}

		return 0;
	}

	const VertexAttribute* VertexBuffer::GetElement(const VertexFormat& vertexFormat, VectorType type, const std::string& semantic, unsigned char index)
	{
		for (PODVector<VertexAttribute>::const_iterator i = vertexFormat.attributes.begin(); i != vertexFormat.attributes.end(); ++i)
		{
			if (i->vectorType == type && i->name == semantic && i->semanticIndex == index)
				return &(*i);
		}

		return 0;
	}

	VertexFormat&& VertexBuffer::GetElements(unsigned elementMask)
	{
		VertexFormat ret;
		/*
		for (unsigned i = 0; i < MAX_LEGACY_VERTEX_ELEMENTS; ++i)
		{
			if (elementMask & (1 << i))
				ret.push_back(LEGACY_VERTEXELEMENTS[i]);
		}*/

		return std::move(ret);
	}

	unsigned VertexBuffer::GetVertexSize(unsigned elementMask)
	{
		unsigned size = 0;
		/*
		for (unsigned i = 0; i < MAX_LEGACY_VERTEX_ELEMENTS; ++i)
		{
			if (elementMask & (1 << i))
				size += ELEMENT_TYPESIZES[LEGACY_VERTEXELEMENTS[i].type_];
		}*/

		return size;
	}

	void VertexBuffer::Release()
	{
		Unlock();

		GraphicsBuffer::Release();
	}

	bool VertexBuffer::SetData(const void* data)
	{	
		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for vertex buffer data");
			return false;
		}

		if (!elementSize_)
		{
			UNIQUE_LOGERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}
		/*
		const bgfx::Memory* mem = nullptr;

		if (shadowData_)
		{
			if (data != shadowData_.Get())
				memcpy(shadowData_.Get(), data, elementCount_ * elementSize_);

			mem = bgfx::makeRef(data, elementCount_ * elementSize_);
		}
		else
		{
			mem = bgfx::copy(data, elementCount_ * elementSize_);
		}

		if (IsDynamic())
		{
			bgfx::updateDynamicVertexBuffer({ handle_ }, 0, mem);
		}
		else
		{
			if (handle_ != bgfx::kInvalidHandle)
			{
				Release();
			}

			bgfx::VertexDecl vertexDecl;
			ToVertexDecl(elements_, vertexDecl);

			handle_ = bgfx::createVertexBuffer(mem, vertexDecl).idx;
		}*/

		return true;
	}

	bool VertexBuffer::SetDataRange(const void* data, unsigned start, unsigned count, bool discard)
	{
		if (start == 0 && count == elementCount_)
			return SetData(data);

		if (!data)
		{
			UNIQUE_LOGERROR("Null pointer for vertex buffer data");
			return false;
		}

		if (!elementSize_)
		{
			UNIQUE_LOGERROR("Vertex elements not defined, can not set vertex buffer data");
			return false;
		}

		if (start + count > elementCount_)
		{
			UNIQUE_LOGERROR("Illegal range for setting new vertex buffer data");
			return false;
		}

		if (!count)
			return true;
		/*
		const bgfx::Memory* mem = nullptr;
		if (shadowData_)
		{
			if (shadowData_.Get() + start * elementSize_ != data)
				memcpy(shadowData_.Get() + start * elementSize_, data, count * elementSize_);

			mem = bgfx::makeRef(shadowData_.Get(), count * elementSize_);
		}
		else
		{
			mem = bgfx::copy(((unsigned char*)data + start * elementSize_), count * elementSize_);
		}

		if (IsDynamic())
		{
			bgfx::updateDynamicVertexBuffer({ handle_ }, start * elementSize_, mem);
		}
		else
		{
			if (valid())
			{
				Release();
			}

			bgfx::VertexDecl vertexDecl;
			ToVertexDecl(elements_, vertexDecl);
			handle_ = bgfx::createVertexBuffer(mem, vertexDecl).idx;
		}
		*/
		return true;
	}

	void* VertexBuffer::Lock(unsigned start, unsigned count, bool discard)
	{/*
		if (lockState_)
		{
			UNIQUE_LOGERROR("Vertex buffer already locked");
			return 0;
		}

		if (!elementSize_)
		{
			UNIQUE_LOGERROR("Vertex elements not defined, can not lock vertex buffer");
			return 0;
		}

		if (start + count > elementCount_)
		{
			UNIQUE_LOGERROR("Illegal range for locking vertex buffer");
			return 0;
		}

		if (!count)
			return 0;

		lockStart_ = start;
		lockCount_ = count;

		if (shadowData_)
		{
			lockState_ = true;
			return shadowData_.Get() + start * elementSize_;
		}
		else*/
		{
			return 0;
		}
	
	}

	void VertexBuffer::Unlock()
	{/*
		if (lockState_)
		{
			SetDataRange(shadowData_.Get() + lockStart_ * elementSize_, lockStart_, lockCount_);
			lockState_ = false;
		}*/
	}

	bool VertexBuffer::Create()
	{
		Release();

		if (!elementCount_ || !elementSize_ || vertexFormat_.stride == 0)
			return true;
		/*
		if (IsDynamic())
		{
			if (!shadowData_)
				shadowData_ = new unsigned char[elementCount_ * elementSize_];

			bgfx::VertexDecl vertexDecl;

			ToVertexDecl(elements_, vertexDecl);

			const bgfx::Memory* mem = bgfx::makeRef(shadowData_.Get(), elementCount_ * elementSize_);
			handle_ = bgfx::createDynamicVertexBuffer(mem, vertexDecl).idx;

		}
		*/
		return true;
	}

}
