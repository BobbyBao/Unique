#pragma once
#include "GraphicsDefs.h"


namespace Unique
{

	/// This structure describes the rasterizer state and is part of the GraphicsPipelineDesc.
	struct RasterizerState
	{
		/// Determines traingle fill mode, see Diligent::FILL_MODE for details.
		FillMode fillMode;

		/// Determines traingle cull mode, see Diligent::CULL_MODE for details.
		CullMode cullMode;

		/// Determines if a triangle is front- or back-facing. If this parameter is True, 
		/// a triangle will be considered front-facing if its vertices are counter-clockwise 
		/// on the render target and considered back-facing if they are clockwise. 
		/// If this parameter is False, the opposite is true.
		bool      frontCounterClockwise;

		/// Constant value added to the depth of a given pixel.
		int     depthBias;

		/// Maximum depth bias of a pixel.
		/// \warning Depth bias clamp is not available in OpenGL
		float   depthBiasClamp;

		/// Scalar that scales the given pixel's slope before adding to the pixel's depth.
		float   slopeScaledDepthBias;

		/// Enable clipping based on distance.
		/// \warning On DirectX this only disables clipping against far clipping plane,
		///          while on OpenGL this disables clipping against both far and near clip planes.
		bool      depthClipEnable;

		/// Enable scissor-rectangle culling. All pixels outside an active scissor rectangle are culled.
		bool      scissorEnable;

		/// Specifies whether to enable line antialiasing.
		bool      antialiasedLineEnable;

		RasterizerState(FillMode _FillMode = FILL_MODE_SOLID,
			CullMode _CullMode = CULL_MODE_BACK,
			bool      _FrontCounterClockwise = false,
			int     _DepthBias = 0,
			float   _DepthBiasClamp = 0.f,
			float   _SlopeScaledDepthBias = 0.f,
			bool      _DepthClipEnable = true,
			bool      _ScissorEnable = false,
			bool      _AntialiasedLineEnable = false) :
			fillMode(_FillMode),
			cullMode(_CullMode),
			frontCounterClockwise(_FrontCounterClockwise),
			depthBias(_DepthBias),
			depthBiasClamp(_DepthBiasClamp),
			slopeScaledDepthBias(_SlopeScaledDepthBias),
			depthClipEnable(_DepthClipEnable),
			scissorEnable(_ScissorEnable),
			antialiasedLineEnable(_AntialiasedLineEnable)
		{
		}

		bool operator == (const RasterizerState& RHS)const
		{
			return  fillMode == RHS.fillMode              &&
				cullMode == RHS.cullMode              &&
				frontCounterClockwise == RHS.frontCounterClockwise &&
				depthBias == RHS.depthBias             &&
				depthBiasClamp == RHS.depthBiasClamp        &&
				slopeScaledDepthBias == RHS.slopeScaledDepthBias  &&
				depthClipEnable == RHS.depthClipEnable       &&
				scissorEnable == RHS.scissorEnable         &&
				antialiasedLineEnable == RHS.antialiasedLineEnable;
		}
	};
	
	uClassTraits
	(
		RasterizerState,
		"FillMode", self.fillMode,
		"CullMode", self.cullMode,
		"FrontCounterClockwise", self.frontCounterClockwise,
		"DepthBias", self.depthBias,
		"DepthBiasClamp", self.depthBiasClamp,
		"SlopeScaledDepthBias", self.slopeScaledDepthBias,
		"DepthClipEnable", self.depthClipEnable,
		"ScissorEnable", self.scissorEnable,
		"AntialiasedLineEnable", self.antialiasedLineEnable
	);

	struct StencilOpDesc
	{
		StencilOp          StencilFailOp;
		StencilOp          StencilDepthFailOp;
		StencilOp          StencilPassOp;
		ComparisonFunc	StencilFunc;
		StencilOpDesc() :
			StencilFailOp(STENCIL_OP_KEEP),
			StencilDepthFailOp(STENCIL_OP_KEEP),
			StencilPassOp(STENCIL_OP_KEEP),
			StencilFunc(COMPARISON_FUNC_ALWAYS)
		{}

		/// Tests if two structures are equivalent

		/// \param [in] rhs - reference to the structure to perform comparison with
		/// \return 
		/// - True if all members of the two structures are equal.
		/// - False otherwise
		bool operator == (const StencilOpDesc& rhs)const
		{
			return StencilFailOp == rhs.StencilFailOp      &&
				StencilDepthFailOp == rhs.StencilDepthFailOp &&
				StencilPassOp == rhs.StencilPassOp      &&
				StencilFunc == rhs.StencilFunc;
		}
	};

	struct DepthStencilState
	{
		bool                DepthEnable;
		bool                DepthWriteEnable;
		ComparisonFunc		DepthFunc;
		bool                StencilEnable;
		byte				StencilReadMask;
		byte				StencilWriteMask;

		/// Identify stencil operations for the front-facing triangles, see Diligent::StencilOpDesc.
		StencilOpDesc      FrontFace;

		/// Identify stencil operations for the back-facing triangles, see Diligent::StencilOpDesc.
		StencilOpDesc      BackFace;

		DepthStencilState(bool _DepthEnable = true,
			bool                _DepthWriteEnable = true,
			ComparisonFunc _DepthFunc = COMPARISON_FUNC_LESS,
			bool                _StencilEnable = false,
			byte               _StencilReadMask = 0xFF,
			byte               _StencilWriteMask = 0xFF) :
			DepthEnable(_DepthEnable),
			DepthWriteEnable(_DepthWriteEnable),
			DepthFunc(_DepthFunc),
			StencilEnable(_StencilEnable),
			StencilReadMask(_StencilReadMask),
			StencilWriteMask(_StencilWriteMask)
		{}

		/// Tests if two structures are equivalent

		/// \param [in] rhs - reference to the structure to perform comparison with
		/// \return 
		/// - True if all members of the two structures are equal.
		/// - False otherwise
		bool operator == (const DepthStencilState& rhs)const
		{
			return  DepthEnable == rhs.DepthEnable      &&
				DepthWriteEnable == rhs.DepthWriteEnable &&
				DepthFunc == rhs.DepthFunc        &&
				StencilEnable == rhs.StencilEnable    &&
				StencilReadMask == rhs.StencilReadMask  &&
				StencilWriteMask == rhs.StencilWriteMask &&
				FrontFace == rhs.FrontFace        &&
				BackFace == rhs.BackFace;
		}
	};

	uClassTraits
	(
		StencilOpDesc,
		"StencilFailOp", self.StencilFailOp,
		"StencilDepthFailOp", self.StencilDepthFailOp,
		"StencilPassOp", self.StencilPassOp,
		"StencilFunc", self.StencilFunc
	);

	uClassTraits
	(
		DepthStencilState,
		"DepthEnable", self.DepthEnable,
		"DepthWriteEnable", self.DepthWriteEnable,
		"ComparisonFunction", self.DepthFunc,
		"StencilEnable", self.StencilEnable,
		"StencilReadMask", self.StencilReadMask,
		"StencilWriteMask", self.StencilWriteMask,
		"FrontFace", self.FrontFace,
		"BackFace", self.BackFace
	);

	struct RenderTargetBlendDesc
	{
		/// Enable or disable blending for this render target.
		bool            blendEnable;

		/// Enable or disable a logical operation for this render target.
		bool			logicOperationEnable;

		/// Specifies the blend factor to apply to the RGB value output from the pixel shader
		BlendFactor    srcBlend;

		/// Specifies the blend factor to apply to the RGB value in the render target
		BlendFactor    destBlend;

		/// Defines how to combine the source and destination RGB values
		/// after applying the SrcBlend and DestBlend factors.
		BlendOperation blendOp;

		/// Specifies the blend factor to apply to the alpha value output from the pixel shader.
		/// Blend factors that end in _COLOR are not allowed. 
		BlendFactor    srcBlendAlpha;

		/// Specifies the blend factor to apply to the alpha value in the render target.
		/// Blend factors that end in _COLOR are not allowed. 
		BlendFactor    destBlendAlpha;

		/// Defines how to combine the source and destination alpha values
		/// after applying the SrcBlendAlpha and DestBlendAlpha factors.
		BlendOperation blendOpAlpha;

		/// Defines logical operation for the render target.
		LogicOperation logicOp;

		/// Render target write mask
		uint          renderTargetWriteMask;

		RenderTargetBlendDesc() :
			blendEnable     ( false ),
			logicOperationEnable(false),
			srcBlend        ( BLEND_FACTOR_ONE ),
			destBlend       ( BLEND_FACTOR_ZERO ),
			blendOp         ( BLEND_OPERATION_ADD ),
			srcBlendAlpha   ( BLEND_FACTOR_ONE ),
			destBlendAlpha  ( BLEND_FACTOR_ZERO ),
			blendOpAlpha    ( BLEND_OPERATION_ADD ),
			logicOp			( LOGIC_OP_NOOP ),
			renderTargetWriteMask( COLOR_MASK_ALL )
		{}


		/// Comparison operator tests if two structures are equivalent

		/// \param [in] rhs - reference to the structure to perform comparison with
		/// \return 
		/// - True if all members of the two structures are equal.
		/// - False otherwise
		bool operator == (const RenderTargetBlendDesc& rhs)const
		{
			return blendEnable           == rhs.blendEnable    &&
				   logicOperationEnable	 == rhs.logicOperationEnable &&
				   srcBlend              == rhs.srcBlend       &&
				   destBlend             == rhs.destBlend      &&
				   blendOp               == rhs.blendOp        &&
				   srcBlendAlpha         == rhs.srcBlendAlpha  &&
				   destBlendAlpha        == rhs.destBlendAlpha &&
				   blendOpAlpha          == rhs.blendOpAlpha   &&
				   logicOp               == rhs.logicOp		   &&
				   renderTargetWriteMask == rhs.renderTargetWriteMask;
		}
	};

	/// Blend state description

	/// This structure describes the blend state and is part of the GraphicsPipelineDesc.
	struct BlendState
	{
		/// Specifies whether to use alpha-to-coverage as a multisampling technique 
		/// when setting a pixel to a render target. Default value: False.
		bool AlphaToCoverageEnable;

		/// Specifies whether to enable independent blending in simultaneous render targets. 
		/// If set to False, only RenderTargets[0] is used. Default value: False.
		bool IndependentBlendEnable;

		/// Constant member defining the maximum number of render targets
		static constexpr int MaxRenderTargets = 8;

		/// An array of RenderTargetBlendDesc structures that describe the blend 
		/// states for render targets
		RenderTargetBlendDesc RenderTargets[MaxRenderTargets];

		/// Constructor initializes structure members with default values

		/// Member                | Default value
		/// ----------------------|--------------
		/// AlphaToCoverageEnable | False
		/// IndependentBlendEnable| False
		/// 
		/// Members of RenderTargets[] are initialized with default values by 
		/// RenderTargetBlendDesc::RenderTargetBlendDesc()
		BlendState() : 
			AlphaToCoverageEnable(false),
			IndependentBlendEnable(false)
		{
		}
    
		/// Comparison operator tests if two structures are equivalent

		/// \param [in] RHS - reference to the structure to perform comparison with
		/// \return 
		/// - True if all members are of the two structures equal. 
		///   \note The operator performs *bitwise comparison* of the two structures. 
		///   That is if for instance both structures have IndependentBlendEnable set to False, 
		///   but differ in render target other than 0, the operator will return False 
		///   even though the two blend states created from these structures will be identical.
		/// - False otherwise
		bool operator == (const BlendState& RHS)const
		{
			bool bRTsEqual = true;
			for( int i = 0; i < MaxRenderTargets; ++i )
				if( !(RenderTargets[i] == RHS.RenderTargets[i]) )
				{
					bRTsEqual = false;
					break;
				}

			return  bRTsEqual &&
					AlphaToCoverageEnable == RHS.AlphaToCoverageEnable &&
					IndependentBlendEnable== RHS.IndependentBlendEnable;
		}
	};
}