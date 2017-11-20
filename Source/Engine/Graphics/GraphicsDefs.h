#pragma once
#include <LLGL/LLGL.h>
#include "../Serialize/SerializeTraits.h"

namespace Unique
{
	template<>
	class SerializeTraits<LLGL::ShaderType> : public SerializeTraitsEnum<LLGL::ShaderType>
	{
	public:
		typedef LLGL::ShaderType value_type;

		template<class TransferFunction> 
		inline static void Transfer(value_type& data, TransferFunction& transfer)
		{
			static const char* shaderType[] =
			{
				"VertexShader", "HullShader", "DomainShader", "GeometryShader",
				"FragmentShader", "ComputeShader"
			};

			TransferEnum<TransferFunction>(data, shaderType, 6, transfer);
		}

	};
}