#include "Precompiled.h"
#include "../Graphics.h"
#include "VertexBuffer.h"
#include "../../Math/MathDefs.h"
#include <LLGL/Utility.h>
#include "../../DebugNew.h"

namespace Unique
{
	extern UPtr<LLGL::RenderSystem>        renderer;
	
	VertexBuffer::VertexBuffer()
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	bool VertexBuffer::Create(unsigned vertexCount, const VertexFormat& elements, long flag, const ByteArray& data)
	{
		elementCount_ = vertexCount;
		flags_ = flag;
	
		if (data.empty())
		{
			data_.resize(elementCount_ * elementSize_);
		}
		else
			data_ = data;

		return GraphicsBuffer::Create();
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

	bool VertexBuffer::CreateImpl()
	{
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
