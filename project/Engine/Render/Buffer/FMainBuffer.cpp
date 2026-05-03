#include "FMainBuffer.h"
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

const std::array<DXGI_FORMAT, FMainBuffer::kLayoutCount> FMainBuffer::kFormats = {
	FBaseBuffer::kColorFormat, //!< Scene
	FBaseBuffer::kColorFormat, //!< Canvas
};

//- Format
// Scene:  [FBaseBuffer::kColorFormat] float4 color
// Canvas: [FBaseBuffer::kColorFormat] float4 color

////////////////////////////////////////////////////////////////////////////////////////////
// FMainBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FMainBuffer::Create(const Vector2ui& resolution) {
	for (size_t i = 0; i < kLayoutCount; ++i) {

		FRenderTexture::Option option = {};
		option.resolution = resolution;
		option.format     = kFormats[i];

		buffers_[i].Create(option);

		std::string name = "FMainBuffer | ";
		name += magic_enum::enum_name(static_cast<FMainBuffer::Layout>(i));
		buffers_[i].SetName(name);
	}
}
