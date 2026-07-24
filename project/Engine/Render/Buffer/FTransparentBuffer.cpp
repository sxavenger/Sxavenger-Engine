#include "FTransparentBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FTransparentBuffer::kLayoutCount> FTransparentBuffer::kFormats = {
	FBaseBuffer::kColorFormat, //!< Accumulate
	DXGI_FORMAT_R16_UNORM,     //!< Revealage
};

//- Format
// Accumulation: [FBaseBuffer::kColorFormat] float4 accumulation
// Revealage:    [DXGI_FORMAT_R16_UNORM]    float transparent

////////////////////////////////////////////////////////////////////////////////////////////
// FTransparentBuffer class
////////////////////////////////////////////////////////////////////////////////////////////

void FTransparentBuffer::Create(const Vector2ui& resolution) {

	buffers_[static_cast<size_t>(Layout::Accumulate)].Create({ resolution, GetFormat(Layout::Accumulate) });
	buffers_[static_cast<size_t>(Layout::Revealage)].Create({ resolution, GetFormat(Layout::Revealage), FRenderTexture::Flag::All, kWhite4<float> });

	for (size_t i = 0; i < kLayoutCount; ++i) {
		// nameの設定
		std::string name = "FTransparentBuffer | ";
		name += magic_enum::enum_name(static_cast<FTransparentBuffer::Layout>(i));
		buffers_[i].SetName(name);
	}

}
