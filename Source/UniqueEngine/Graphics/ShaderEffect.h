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
	struct LayoutElement
	{
		/// Input index of the element, which is specified in the vertex shader.
		uint inputIndex_;

		/// Buffer slot index that this element is read from.
		uint BufferSlot;

		/// Number of components in the element. Allowed values are 1, 2, 3, and 4.
		uint NumComponents;

		/// Type of the element components, see Diligent::VALUE_TYPE for details.
		ValueType valueType;

		/// For signed and unsigned integer value types 
		/// (VT_INT8, VT_INT16, VT_INT32, VT_UINT8, VT_UINT16, VT_UINT32)
		/// indicates if the value should be normalized to [-1,+1] or 
		/// [0, 1] range respectively. For floating point types
		/// (VT_FLOAT16 and VT_FLOAT32), this member is ignored.
		bool IsNormalized;

		/// Relative offset, in bytes, to the element bits.
		/// If this value is zero, the offset will be computed automatically
		/// assuming that all previous elements in the same buffer slot a tightly packed.
		/// Overlapping elements are not allowed.
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
		{}
	};

	struct InputLayout
	{
		Vector<LayoutElement> layoutElements_;

		uClass("LayoutElement", layoutElements_)
	};

	class Pass : public Object
	{
		uRTTI(Pass, Object)
	public:
		Pass(const String& name = "");
		~Pass();

		uint GetMask(Shader* shader, const String& defs);

		PipelineState* GetPipeline(Shader* shader, const String & defs);

		PipelineState* GetPipeline(Shader* shader, unsigned defMask);

		bool Prepare();

		String					name_;
		uint					passIndex_ = 0;
		RasterizerState			rasterizerState_;
		DepthStencilState		depthState_;
		BlendState		blendState_;
		InputLayout				inputLayout_;
		ShaderStage				shaderStage_[6];

	private:
		SPtr<ShaderVariation>	GetShaderVariation(Shader& shader, const ShaderStage& shaderStage, uint defs);
		Vector<String>			allDefs_;
		uint					allMask_ = 0;
		HashMap<uint, SPtr<PipelineState>> cachedPass_;
		HashMap<uint64_t, SPtr<ShaderVariation>> cachedShaders_;

		friend class Shader;
		friend class ShaderVariation;
		friend class PipelineState;
	};

	class ShaderProperties
	{
	public:	
		Vector<Uniform>		uniforms_;
		Vector<TextureSlot> textureSlots_;

		uClass("Uniform", uniforms_, "TextureSlot", textureSlots_)
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
		PipelineState* GetPipeline(const String& passName, const String & defs);

		inline const auto& GetUniforms() const { return shaderProperties_.uniforms_; }
		inline const auto& GetTextureSlots() const { return shaderProperties_.textureSlots_; }

		static unsigned GetPassIndex(const String& passName);
		static String GetShaderPath(DeviceType deviceType);
		static Vector<String> SplitDef(const String& defs);

		/// Index for base pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned basePassIndex;
		/// Index for alpha pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned alphaPassIndex;
		/// Index for prepass material pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned materialPassIndex;
		/// Index for deferred G-buffer pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned deferredPassIndex;
		/// Index for per-pixel light pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned lightPassIndex;
		/// Index for lit base pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned litBasePassIndex;
		/// Index for lit alpha pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned litAlphaPassIndex;
		/// Index for shadow pass. Initialized once GetPassIndex() has been called for the first time.
		static unsigned shadowPassIndex;

		const ShaderProperties& GetProperties() const { return shaderProperties_; }
	private:
		String				shaderName_;
		ShaderProperties	shaderProperties_;
		Vector<SPtr<Pass>>	passes_;

		/// Pass index assignments.
		static HashMap<String, unsigned> passIndices;
	};

}
