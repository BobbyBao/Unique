#pragma once
#include "GraphicsDefs.h"
#include <DepthStencilState.h>
#include <BlendState.h>
#include <RasterizerState.h>


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


}