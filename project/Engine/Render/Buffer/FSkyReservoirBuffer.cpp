#include "FSkyReservoirBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/System.h>

//* lib
#include <Lib/Adapter/String/EncodedString.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FSkyReservoirBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FSkyReservoirBuffer::Create(const Vector2ui& resolution) {
	for (size_t i = 0; i < kLayoutReservoirCount; ++i) {
		reservoirs_[i].Create(System::GetDxDevice(), resolution.x * resolution.y);

		//!< nameの設定
		std::string name = "FSkyReservoirBuffer | Reservoir | ";
		name += magic_enum::enum_name(static_cast<LayoutReservoir>(i));
		reservoirs_[i].SetName(EncodedString::Convert(name).c_str());
	}

	for (size_t i = 0; i < kLayoutTextureCount; ++i) {

		FRenderTexture::Option option = {};
		option.resolution = resolution;
		option.format     = FBaseBuffer::kColorFormat;

		buffers_[i].Create(option);

		 //!< nameの設定
		std::string name = "FSkyReservoirBuffer | Texture | ";
		name += magic_enum::enum_name(static_cast<LayoutTexture>(i));
		buffers_[i].SetName(EncodedString::Convert(name).c_str());
	}
}
