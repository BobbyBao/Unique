#pragma once
#include "Resource/Resource.h"
#include "ShaderVariation.h"
#include "ShaderVariable.h"
#include "RenderState.h"

namespace Unique
{
	class Shader;
	class PipelineState;

	/// Description of a single element of the input layout
	struct LayoutElement : public Diligent::LayoutElement
	{/*
		static constexpr uint AutoOffset = static_cast<uint>(-1);
		static constexpr uint AutoStride = static_cast<uint>(-1);
		/// Input index of the element, which is specified in the vertex shader.
		uint InputIndex = 0;

		/// Buffer slot index that this element is read from.
		uint BufferSlot = 0;

		/// Number of components in the element. Allowed values are 1, 2, 3, and 4.
		uint NumComponents = 0;

		/// Type of the element components, see Diligent::VALUE_TYPE for details.
		ValueType ValueType = VT_FLOAT32;

		/// For signed and unsigned integer value types 
		/// (VT_INT8, VT_INT16, VT_INT32, VT_UINT8, VT_UINT16, VT_UINT32)
		/// indicates if the value should be normalized to [-1,+1] or 
		/// [0, 1] range respectively. For floating point types
		/// (VT_FLOAT16 and VT_FLOAT32), this member is ignored.
		bool IsNormalized = true;

		/// Relative offset, in bytes, to the element bits.
		/// If this value is set to LayoutElement::AutoOffset (default value), the offset will
		/// be computed automatically by placing the element right after the previous one.
		uint RelativeOffset = AutoOffset;

		/// Stride, in bytes, between two elements, for this buffer slot.
		/// If this value is set to LayoutElement::AutoStride, the stride will be
		/// computed automatically assuming that all elements in the same buffer slot are
		/// packed one after another. If the buffer slot contains multiple layout elements,
		/// they all must specify the same stride or use AutoStride value.
		uint Stride = AutoStride;

		/// Input frequency
		enum FREQUENCY : int
		{
			/// Frequency is undefined.
			FREQUENCY_UNDEFINED = 0,

			/// Input data is per-vertex data.
			FREQUENCY_PER_VERTEX,

			/// Input data is per-instance data.
			FREQUENCY_PER_INSTANCE,

			/// Helper value that stores the total number of frequencies in the enumeration.
			FREQUENCY_NUM_FREQUENCIES
		};
		FREQUENCY Frequency = FREQUENCY_PER_VERTEX;

		/// The number of instances to draw using the same per-instance data before advancing 
		/// in the buffer by one element.
		uint InstanceDataStepRate = 1;

		LayoutElement()noexcept {}

		/// Initializes the structure
		LayoutElement(uint     _InputIndex,
			uint     _BufferSlot,
			uint     _NumComponents,
			Unique::ValueType _ValueType,
			bool       _IsNormalized = LayoutElement{}.IsNormalized,
			uint     _RelativeOffset = LayoutElement{}.RelativeOffset,
			uint     _Stride = LayoutElement{}.Stride,
			FREQUENCY  _Frequency = LayoutElement{}.Frequency,
			uint     _InstanceDataStepRate = LayoutElement{}.InstanceDataStepRate)noexcept :
			InputIndex(_InputIndex),
			BufferSlot(_BufferSlot),
			NumComponents(_NumComponents),
			ValueType(_ValueType),
			IsNormalized(_IsNormalized),
			RelativeOffset(_RelativeOffset),
			Stride(_Stride),
			Frequency(_Frequency),
			InstanceDataStepRate(_InstanceDataStepRate)
		{}

		/*
		/// Input index of the element, which is specified in the vertex shader.
		uint inputIndex_;
		/// Buffer slot index that this element is read from.
		uint BufferSlot;
		/// Number of components in the element. Allowed values are 1, 2, 3, and 4.
		uint NumComponents;
		/// Type of the element components, see Diligent::VALUE_TYPE for details.
		ValueType valueType;
		bool IsNormalized;
		uint RelativeOffset;

		/// Input frequency
		enum FREQUENCY : int
		{
			/// Frequency is undefined.
			FREQUENCY_UNDEFINED = 0,

			/// Input data is per-vertex data.
			FREQUENCY_PER_VERTEX,

			/// Input data is per-instance data.
			FREQUENCY_PER_INSTANCE,

			/// Helper value that stores the total number of frequencies in the enumeration.
			FREQUENCY_NUM_FREQUENCIES
		}Frequency;

		/// The number of instances to draw using the same per-instance data before advancing 
		/// in the buffer by one element.
		uint InstanceDataStepRate;

		/// Initializes the structure
		LayoutElement(uint _InputIndex = 0,
			uint _BufferSlot = 0,
			uint _NumComponents = 0,
			ValueType _ValueType = VT_FLOAT32,
			bool _IsNormalized = false,
			uint _RelativeOffset = 0,
			FREQUENCY _Frequency = FREQUENCY_PER_VERTEX,
			uint _InstanceDataStepRate = 1) :
			inputIndex_(_InputIndex),
			BufferSlot(_BufferSlot),
			NumComponents(_NumComponents),
			valueType(_ValueType),
			IsNormalized(_IsNormalized),
			RelativeOffset(_RelativeOffset),
			Frequency(_Frequency),
			InstanceDataStepRate(_InstanceDataStepRate)
		{}*/

		bool operator == ( const LayoutElement& rhs) const
		{
			return std::memcmp(this, &rhs, sizeof(*this)) == 0;
		}

	};

	struct InputLayout
	{
		Vector<LayoutElement> layoutElements_;
		
		bool operator ==( const InputLayout& rhs) const
		{
			return layoutElements_ == rhs.layoutElements_;
		}

		uClass("LayoutElement", layoutElements_)
	};
	
	class ShaderProperties
	{
	public:	
		Vector<Uniform>		uniforms_;
		Vector<TextureSlot> textureSlots_;
		
		bool operator == ( const ShaderProperties& rhs) const
		{
			return uniforms_ == rhs.uniforms_ && textureSlots_ == rhs.textureSlots_;
		}

		uClass("Uniform", uniforms_, "TextureSlot", textureSlots_)
	};

	class Pass : public Object
	{
		uRTTI(Pass, Object)
	public:
		Pass(const String& name = "");
		~Pass();

		uint64 GetMask(Shader* shader, const String& interDefs, const String& defs);
		PipelineState* GetPipeline(Shader* shader, const String& interDefs, const String & defs);
		PipelineState* GetPipeline(Shader* shader, uint64 defMask);
		bool Prepare();
		
		inline const auto& GetUniforms() const { return shaderProperties_.uniforms_; }
		inline const auto& GetTextureSlots() const { return shaderProperties_.textureSlots_; }
		const ShaderProperties& GetProperties() const { return shaderProperties_; }

		String					name_;
		uint					passIndex_ = 0;
		RasterizerState			rasterizerState_;
		DepthStencilState		depthState_;
		BlendState				blendState_;
		InputLayout				inputLayout_;
		ShaderStage				shaderStage_[6];

	private:
		SPtr<ShaderVariation>	GetShaderVariation(Shader& shader, const ShaderStage& shaderStage, uint64 defs);
	
		ShaderProperties		shaderProperties_;

		Vector<String>			allDefs_;
		uint64					allMask_ = 0;

		HashMap<uint64, SPtr<PipelineState>> cachedPipeline_;
		HashMap<uint64, SPtr<ShaderVariation>> cachedShaders_[6];

		friend class Shader;
		friend class ShaderVariation;
		friend class PipelineState;
	};

	class Shader : public Resource
	{
		uRTTI(Shader, Resource)
	public:
		Shader();
		~Shader();

		virtual bool Prepare();
		virtual bool Create();

		Pass* AddPass(const String& name);
		Pass* GetPass(const String & pass);
		Pass* GetPass(unsigned passIndex);

		PipelineState* GetPipeline(const String& passName, const String& interDefs, const String & defs);
		PipelineState * GetPipeline(uint passIndex, const String & interDefs, const String & defs);

	private:
		String				shaderName_;
		Vector<SPtr<Pass>>	passes_;

	};

}
