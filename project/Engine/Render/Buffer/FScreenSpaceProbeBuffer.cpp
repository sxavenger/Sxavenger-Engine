#include "FScreenSpaceProbeBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Core/FRenderCoreLuxGlobalIllumination.h"

//* engine
#include <Engine/System/System.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FScreenSpaceProbeBuffer::kLayoutCount> FScreenSpaceProbeBuffer::kFormats = {
	FBaseBuffer::kColorFormat, //!< Radiance
	FBaseBuffer::kColorFormat, //!< Irradiance
};

//- Format
// Radiance: [FBaseBuffer::kColorFormat] float3 radiance

////////////////////////////////////////////////////////////////////////////////////////////
// FScreenSpaceProbeBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FScreenSpaceProbeBuffer::Create(const Vector2ui& resolution) {

	static const FRenderCoreLuxGlobalIllumination::Setting setting = {}; //!< TODO: どこかで設定できるようにする

	buffers_[static_cast<size_t>(Layout::Radiance)].Create({ resolution / setting.downscale * setting.atlas, kFormats[static_cast<size_t>(Layout::Radiance)] });
	buffers_[static_cast<size_t>(Layout::Irradiance)].Create({ resolution / setting.downscale * setting.atlas, kFormats[static_cast<size_t>(Layout::Irradiance)] });

	for (size_t i = 0; i < kLayoutCount; ++i) {
		// nameの設定
		std::string name = "FScreenSpaceProbeBuffer | ";
		name += magic_enum::enum_name(static_cast<FScreenSpaceProbeBuffer::Layout>(i));
		buffers_[i].SetName(name);
	}
}
