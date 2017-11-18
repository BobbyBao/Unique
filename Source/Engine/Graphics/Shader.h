#pragma once
#include "../Core/Object.h"

namespace Unique
{
	class Shader : public Object
	{
		UNIQUE_OBJECT(Shader, Object)
	public:
		Shader();
		~Shader();

		const String& GetName() const { return name_; }
		void SetName(const String& name) { name_ = name;}

	private:
		String name_;
		String shaderDefines_;
	};

}
