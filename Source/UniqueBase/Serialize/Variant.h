#pragma once

namespace Unique
{
	/// Variant's supported types.
	enum VariantType
	{
		VAR_NONE = 0,
		VAR_INT,
		VAR_BOOL,
		VAR_FLOAT,
		VAR_INT64,
		VAR_DOUBLE,
		VAR_VECTOR2,
		VAR_VECTOR3,
		VAR_VECTOR4,
		VAR_QUATERNION,
		VAR_COLOR,
		VAR_STRING,
		VAR_BUFFER,
		VAR_PTR,
		MAX_VAR_TYPES
	};

	struct VariantData
	{
		Vector4 vec4;
	};

	class Variant
	{
	public:

		template<class T>
		Variant& operator = (const T& val)
		{
			return *this;
		}

		VariantData data_;
	};


}