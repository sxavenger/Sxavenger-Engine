#include "FDepthStencilBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//* lib
#include <Lib/Sxl/Utility.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FDepthStencilBuffer::kLayoutCount> FDepthStencilBuffer::kFormats = {
	FBaseBuffer::kDepthStencilFormat, //!< Scene
	DXGI_FORMAT_D24_UNORM_S8_UINT,    //!< Canvas
};

//- Format
// Scene:  [FBaseBuffer::kDepthStencilFormat] depth, stencil
// Canvas: [DXGI_FORMAT_D24_UNORM_S8_UINT] depth, stencil

////////////////////////////////////////////////////////////////////////////////////////////
// FDepthStencilBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FDepthStencilBuffer::Create(const Vector2ui& resolution) {
	for (size_t i = 0; i < kLayoutCount; ++i) {

		FDepthStencilTexture::Option option = {};
		option.resolution = resolution;
		option.format     = kFormats[i];

		buffers_[i].Create(option);

		std::string name = "FDepthStencilBuffer | ";
		name += magic_enum::enum_name(static_cast<FDepthStencilBuffer::Layout>(i));
		buffers_[i].SetName(name);
	}
}
