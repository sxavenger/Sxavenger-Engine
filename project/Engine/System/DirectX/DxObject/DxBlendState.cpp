#include "DxBlendState.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BlendState class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BlendState::Init() {

	{ //!< None
		D3D12_RENDER_TARGET_BLEND_DESC desc = {};
		desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		descs_[static_cast<uint32_t>(BlendMode::kBlendModeNone)] = desc;
	}

	{ //!< Normal
		D3D12_RENDER_TARGET_BLEND_DESC desc = {};
		desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.BlendEnable           = true;
		desc.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		desc.BlendOp               = D3D12_BLEND_OP_ADD;
		desc.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		desc.SrcBlendAlpha         = D3D12_BLEND_ONE;
		desc.BlendOpAlpha          = D3D12_BLEND_OP_MAX;
		desc.DestBlendAlpha        = D3D12_BLEND_ONE;

		// SrcColor * SrcAlpha + DestColor * (1.0f - SrcAlpha)
		descs_[static_cast<uint32_t>(BlendMode::kBlendModeNormal)] = desc;
	}

	{ //!< Normal Src
		D3D12_RENDER_TARGET_BLEND_DESC desc = {};
		desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.BlendEnable           = true;
		desc.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		desc.BlendOp               = D3D12_BLEND_OP_ADD;
		desc.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
		desc.SrcBlendAlpha         = D3D12_BLEND_ONE;
		desc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.DestBlendAlpha        = D3D12_BLEND_ZERO;

		// SrcColor * SrcAlpha + DestColor * (1.0f - SrcAlpha)
		descs_[static_cast<uint32_t>(BlendMode::kBlendModeNormalSrc)] = desc;
	}

	{ //!< Add
		D3D12_RENDER_TARGET_BLEND_DESC desc = {};
		desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.BlendEnable           = true;
		desc.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		desc.BlendOp               = D3D12_BLEND_OP_ADD;
		desc.DestBlend             = D3D12_BLEND_ONE;
		desc.SrcBlendAlpha         = D3D12_BLEND_ZERO;
		desc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.DestBlendAlpha        = D3D12_BLEND_ONE;

		// SrcColor * SrcAlpha + DestColor
		descs_[static_cast<uint32_t>(BlendMode::kBlendModeAdd)] = desc;
	}

	{ //!< Subtract
		D3D12_RENDER_TARGET_BLEND_DESC desc = {};
		desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.BlendEnable           = true;
		desc.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
		desc.BlendOp               = D3D12_BLEND_OP_REV_SUBTRACT;
		desc.DestBlend             = D3D12_BLEND_ONE;
		desc.SrcBlendAlpha         = D3D12_BLEND_ZERO;
		desc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.DestBlendAlpha        = D3D12_BLEND_ONE;

		// DestColor - SrcColor * SrcAlpha
		descs_[static_cast<uint32_t>(BlendMode::kBlendModeSubtract)] = desc;
	}

	{ //!< Multiply
		D3D12_RENDER_TARGET_BLEND_DESC desc = {};
		desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.BlendEnable           = true;
		desc.SrcBlend              = D3D12_BLEND_ZERO;
		desc.BlendOp               = D3D12_BLEND_OP_ADD;
		desc.DestBlend             = D3D12_BLEND_SRC_COLOR;
		desc.SrcBlendAlpha         = D3D12_BLEND_ZERO;
		desc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.DestBlendAlpha        = D3D12_BLEND_ONE;

		// SrcColor * DestColor
		descs_[static_cast<uint32_t>(BlendMode::kBlendModeMultiply)] = desc;
	}

	{ //!< Screen
		D3D12_RENDER_TARGET_BLEND_DESC desc = {};
		desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		desc.BlendEnable           = true;
		desc.SrcBlend              = D3D12_BLEND_INV_DEST_COLOR;
		desc.BlendOp               = D3D12_BLEND_OP_ADD;
		desc.DestBlend             = D3D12_BLEND_ONE;
		desc.SrcBlendAlpha         = D3D12_BLEND_ZERO;
		desc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		desc.DestBlendAlpha        = D3D12_BLEND_ONE;

		// (1.0f - DescColor) * SrcColor + DescColor
		descs_[static_cast<uint32_t>(BlendMode::kBlendModeScreen)] = desc;
	}
}

void BlendState::Term() {
}
