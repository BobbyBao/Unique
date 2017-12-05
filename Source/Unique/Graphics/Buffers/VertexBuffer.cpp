#include "Precompiled.h"
#include "../Graphics.h"
#include "VertexBuffer.h"
#include "../../Math/MathDefs.h"
#include <LLGL/Utility.h>
#include "../../DebugNew.h"

namespace Unique
{
	uEnumTraits(VectorType, 
		"Float", "Float2", "Float3", "Float4",
		"Double", "Double2", "Double3", "Double4",
		"Int", "Int2", "Int3", "Int4",
		"UInt", "UInt2", "UInt3", "UInt4")

	uClassTraits(VertexAttribute,
		"SemanticName", self.name,
		"SemanticIndex", self.semanticIndex,
		"VectorType", self.vectorType,
		"InstanceDivisor", self.instanceDivisor)

	uClassTraits(VertexFormat, "Attributes", self.attributes)

	uObject(VertexBuffer)
	{
		uFactory("Graphics")
			uAttribute("VertexFormat", vertexFormat_)
			uAttribute("Data", data_)
	}

	extern UPtr<LLGL::RenderSystem>        renderer;
	
	VertexBuffer::VertexBuffer()
	{
	}

	VertexBuffer::~VertexBuffer()
	{
	}

	bool VertexBuffer::Create(uint vertexCount, const VertexFormat& vertexFormat, long flag, void* data)
	{
		elementSize_ = vertexFormat.stride;
		elementCount_ = vertexCount;
		vertexFormat_ = vertexFormat;
		flags_ = flag;

		data_.resize(elementCount_ * elementSize_);
		if (data)
		{
			std::memcpy(data_.data(), data, elementCount_ * elementSize_);
		}
	

		return GraphicsBuffer::Create();
	}

	bool VertexBuffer::CreateImpl()
	{
		if (!elementCount_ || !elementSize_)
			return false;
		
		try
		{
			handle_ = renderer->CreateBuffer(VertexBufferDesc((uint)data_.size(), vertexFormat_, flags_), data_.data());
		}
		catch (std::exception& e)
		{
			UNIQUE_LOGERRORF(e.what());
		}

		return handle_ != nullptr;
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


}
