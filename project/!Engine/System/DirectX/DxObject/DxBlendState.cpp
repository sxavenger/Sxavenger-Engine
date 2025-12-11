#include "DxBlendState.h"
DXOBJECT_USING

//=========================================================================================
// static const variables
//=========================================================================================

std::array<D3D12_RENDER_TARGET_BLEND_DESC, magic_enum::enum_count<BlendMode>()> BlendState::descs_ = {
	D3D12_RENDER_TARGET_BLEND_DESC{ //!< None
		.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	},

	D3D12_RENDER_TARGET_BLEND_DESC{ //!< Normal
		// SrcColor * SrcAlpha + DestColor * (1 - SrcAlpha)
		.BlendEnable           = true,
		.SrcBlend              = D3D12_BLEND_SRC_ALPHA,
		.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA,
		.BlendOp               = D3D12_BLEND_OP_ADD,
		.SrcBlendAlpha         = D3D12_BLEND_ONE,
		.DestBlendAlpha        = D3D12_BLEND_ONE,
		.BlendOpAlpha          = D3D12_BLEND_OP_ADD,
		.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
		
	},

	D3D12_RENDER_TARGET_BLEND_DESC{ //!< Normal_AlphaMax
		// SrcColor * SrcAlpha + DestColor * (1 - SrcAlpha)
		.BlendEnable           = true,
		.SrcBlend              = D3D12_BLEND_SRC_ALPHA,
		.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA,
		.BlendOp               = D3D12_BLEND_OP_ADD,
		.SrcBlendAlpha         = D3D12_BLEND_ONE,
		.DestBlendAlpha        = D3D12_BLEND_ONE,
		.BlendOpAlpha          = D3D12_BLEND_OP_MAX,
		.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	},

	D3D12_RENDER_TARGET_BLEND_DESC{ //!< Add
		// (SrcColor * SrcAlpha) + DestColor
		.BlendEnable           = true,
		.SrcBlend              = D3D12_BLEND_SRC_ALPHA,
		.DestBlend             = D3D12_BLEND_ONE,
		.BlendOp               = D3D12_BLEND_OP_ADD,
		.SrcBlendAlpha         = D3D12_BLEND_ZERO,
		.DestBlendAlpha        = D3D12_BLEND_ONE,
		.BlendOpAlpha          = D3D12_BLEND_OP_ADD,
		.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	},

	D3D12_RENDER_TARGET_BLEND_DESC{ //!< Subtract
		// DestColor - (SrcColor * SrcAlpha)
		.BlendEnable           = true,
		.SrcBlend              = D3D12_BLEND_SRC_ALPHA,
		.DestBlend             = D3D12_BLEND_ONE,
		.BlendOp               = D3D12_BLEND_OP_REV_SUBTRACT,
		.SrcBlendAlpha         = D3D12_BLEND_ZERO,
		.DestBlendAlpha        = D3D12_BLEND_ONE,
		.BlendOpAlpha          = D3D12_BLEND_OP_ADD,
		.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	},

	D3D12_RENDER_TARGET_BLEND_DESC{ //!< Multiply
		// SrcColor * DestColor
		.BlendEnable           = true,
		.SrcBlend              = D3D12_BLEND_ZERO,
		.DestBlend             = D3D12_BLEND_SRC_COLOR,
		.BlendOp               = D3D12_BLEND_OP_ADD,
		.SrcBlendAlpha         = D3D12_BLEND_ZERO,
		.DestBlendAlpha        = D3D12_BLEND_ONE,
		.BlendOpAlpha          = D3D12_BLEND_OP_ADD,
		.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	},

	D3D12_RENDER_TARGET_BLEND_DESC{ //!< Screen
		// (1 - DescColor) * SrcColor + DescColor
		.BlendEnable           = true,
		.SrcBlend              = D3D12_BLEND_INV_DEST_COLOR,
		.DestBlend             = D3D12_BLEND_ONE,
		.BlendOp               = D3D12_BLEND_OP_ADD,
		.SrcBlendAlpha         = D3D12_BLEND_ZERO,
		.DestBlendAlpha        = D3D12_BLEND_ONE,
		.BlendOpAlpha          = D3D12_BLEND_OP_ADD,
		.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	}

};
