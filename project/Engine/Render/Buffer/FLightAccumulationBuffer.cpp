#include "FLightAccumulationBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FLightAccumulationBuffer::kLayoutCount> FLightAccumulationBuffer::kFormats = {
	FBaseBuffer::kColorFormat, //!< Direct
	FBaseBuffer::kColorFormat, //!< Indirect
};

//- Format
// Direct:   [FBaseBuffer::kColorFormat] float3 direct lighting
// Indirect: [FBaseBuffer::kColorFormat] float3 indirect lighting

////////////////////////////////////////////////////////////////////////////////////////////
// FLightAccumulationBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FLightAccumulationBuffer::Create(const Vector2ui& resolution) {
	for (size_t i = 0; i < kLayoutCount; ++i) {

		FRenderTexture::Option option = {};
		option.resolution = resolution;
		option.format     = kFormats[i];

		buffers_[i].Create(option);

		std::string name = "FLightAccumulationBuffer | ";
		name += magic_enum::enum_name(static_cast<FLightAccumulationBuffer::Layout>(i));
		buffers_[i].SetName(name);
	}
}
