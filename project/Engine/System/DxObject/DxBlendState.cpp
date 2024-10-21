#include "DxBlendState.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DxObject set static
#include "DxGraphicsPipeline.h"

////////////////////////////////////////////////////////////////////////////////////////////
// BlendState methods
////////////////////////////////////////////////////////////////////////////////////////////

void BlendState::Init() {

	// none
	{
		D3D12_BLEND_DESC desc = {};
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		blendDesc_[BlendMode::kBlendModeNone] = desc;
	}

	// normal
	{
		D3D12_BLEND_DESC desc = {};
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.RenderTarget[0].BlendEnable           = TRUE;
		desc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;

		// SrcColor * SrcAlpha + DestColor * (1.0f - SrcAlpha)
		blendDesc_[BlendMode::kBlendModeNormal] = desc;
	}

	// add
	{
		D3D12_BLEND_DESC desc = {};
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.RenderTarget[0].BlendEnable           = TRUE;
		desc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlend             = D3D12_BLEND_ONE;
		desc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;

		// SrcColor * SrcAlpha + DescColor
		blendDesc_[BlendMode::kBlendModeAdd] = desc;
	}

	// subtract
	{
		D3D12_BLEND_DESC desc = {};
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.RenderTarget[0].BlendEnable           = TRUE;
		desc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.RenderTarget[0].DestBlend             = D3D12_BLEND_ONE;
		desc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;

		// DescColor - SrcColor * SrcAlpha
		blendDesc_[BlendMode::kBlendModeSubtract] = desc;
	}

	// multiply
	{
		D3D12_BLEND_DESC desc = {};
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.RenderTarget[0].BlendEnable           = TRUE;
		desc.RenderTarget[0].SrcBlend              = D3D12_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlend             = D3D12_BLEND_SRC_COLOR;
		desc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;

		// SrcColor * DescColor
		blendDesc_[BlendMode::kBlendModeMultily] = desc;
	}

	// Screen
	{
		D3D12_BLEND_DESC desc = {};
		desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.RenderTarget[0].BlendEnable           = TRUE;
		desc.RenderTarget[0].SrcBlend              = D3D12_BLEND_INV_DEST_COLOR;
		desc.RenderTarget[0].BlendOp               = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlend             = D3D12_BLEND_ONE;
		desc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ZERO;

		// (1.0f - DescColor) * SrcColor + DescColor
		blendDesc_[BlendMode::kBlendModeScreen] = desc;
	}

	// setting ptr
	GraphicsPipeline::SetBlendState(this);

}

void BlendState::Term() {
}
