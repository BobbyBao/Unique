#pragma once
#include "resource/Resource.h"
#include "GraphicsDefs.h"
#include "GPUObject.h"
#include "ShaderUtil.h"

namespace Unique
{
	enum ShaderType : int
	{
		SHADER_TYPE_UNKNOWN     = 0x000, ///< Unknown shader type
		SHADER_TYPE_VERTEX      = 0x001, ///< Vertex shader
		SHADER_TYPE_PIXEL       = 0x002, ///< Pixel (fragment) shader
		SHADER_TYPE_GEOMETRY    = 0x004, ///< Geometry shader
		SHADER_TYPE_HULL        = 0x008, ///< Hull (tessellation control) shader
		SHADER_TYPE_DOMAIN      = 0x010, ///< Domain (tessellation evaluation) shader
		SHADER_TYPE_COMPUTE     = 0x020  ///< Compute shader
	};

	enum ShaderProfile : int
	{
		SHADER_PROFILE_DEFAULT = 0,
		SHADER_PROFILE_DX_4_0,
		SHADER_PROFILE_DX_5_0,
		SHADER_PROFILE_DX_5_1,
		SHADER_PROFILE_GL_4_2
	};
	
	/// Describes shader variable type that is used by ShaderVariableDesc
	enum ShaderVariableType : int
	{
		/// Shader variable is constant across all shader instances.
		/// It must be set *once* directly through IShader::BindResources() or through 
		/// the shader variable.
		SHADER_VARIABLE_TYPE_STATIC = 0, 

		/// Shader variable is constant across shader resource bindings instance (see IShaderResourceBinding).
		/// It must be set *once* through IShaderResourceBinding::BindResources() or through
		/// the shader variable. It cannot be set through IShader interface
		SHADER_VARIABLE_TYPE_MUTABLE,

		/// Shader variable is dynamic. It can be set multiple times for every instance of shader resource 
		/// bindings (see IShaderResourceBinding). It cannot be set through IShader interface
		SHADER_VARIABLE_TYPE_DYNAMIC,

		/// Total number of shader variable types
		SHADER_VARIABLE_TYPE_NUM_TYPES
	};

	/// Describes shader variable
	struct ShaderVariableDesc
	{
		/// Shader variable name
		const char *name_;

		/// Shader variable type. See Diligent::SHADER_VARIABLE_TYPE for a list of allowed types
		ShaderVariableType type_;
		ShaderVariableDesc(const char *name = nullptr, ShaderVariableType type = SHADER_VARIABLE_TYPE_STATIC) : 
			name_(name),
			type_(type)
		{}
	};

	class Shader;
	class Pass;

	uEnum(ShaderProfile, "DEFAULT", "DX_4_0", "DX_5_0", "DX_5_1", "GL_4_2")

	struct ShaderStage
	{
		ShaderStage(ShaderType type = ShaderType::SHADER_TYPE_UNKNOWN)
			: shaderType_(type)
		{
		}

		ShaderStage(ShaderType type, const String& filename) :
			shaderType_(type)
		{
		}

		ShaderStage(
			ShaderType type, const String& filename, const String& entryPoint, const ShaderProfile& target) :
			shaderType_(type),
			entryPoint_{ entryPoint },
			shaderProfile_{ target }
		{
		}

		operator bool()
		{
			return !source_.Empty();
		}

		uClass("EntryPoint", entryPoint_, "ShaderProfile", shaderProfile_, "Defines", defines_, "Source", source_);

		ShaderType		shaderType_;
		String			entryPoint_;
		ShaderProfile	shaderProfile_ = SHADER_PROFILE_DEFAULT;
		String			defines_;
		String			source_;
	};

	class ShaderVariation : public RefCounted, public GPUObject
	{
	public:
		ShaderVariation(Shader& shader, const ShaderStage& type, Pass& shaderPass, uint64 defs);

		bool CreateImpl();
		void Reload();
		bool LoadConvertedCode(const String & binaryShaderName);
		bool Convert(const String& binaryShaderName);
		ShaderType GetShaderType() const { return shaderStage_.shaderType_; }

	protected:
		Shader& owner_;
		ShaderStage shaderStage_;
		Pass& shaderPass_;
		String source_;
		/// Defines to use in compiling.
		String defines_;
		bool dirty_ = false;
		ShaderMacroHelper macros_;
		Vector<ShaderVariableDesc> shaderVariableDesc_;

		friend class Graphics;
	};

}
