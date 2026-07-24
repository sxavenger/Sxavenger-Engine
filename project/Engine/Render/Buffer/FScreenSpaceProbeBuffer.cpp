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
	FBaseBuffer::kColorFormat, //!< BRDFRadianceCache
	DXGI_FORMAT_R32G32_UINT,   //!< Moment
	FBaseBuffer::kColorFormat, //!< History
};

//- Format
// BRDFRadianceCache: [FBaseBuffer::kColorFormat] float3 radiance, float pdf
// Moment:            [DXGI_FORMAT_R32_UINT] uint moment
// History:           [FBaseBuffer::kColorFormat] float3 color (もしかしたら不要になるかも)

////////////////////////////////////////////////////////////////////////////////////////////
// FScreenSpaceProbeBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FScreenSpaceProbeBuffer::Create(const Vector2ui& resolution) {

	static const FRenderCoreLuxGlobalIllumination::Setting setting = {}; //!< TODO: どこかで設定できるようにする

	//* cache *//

	buffers_[static_cast<size_t>(Layout::BRDFRadianceCache)].Create({ setting.CalculateResolution(resolution), kFormats[static_cast<size_t>(Layout::BRDFRadianceCache)]});

	buffers_[static_cast<size_t>(Layout::Moment)].Create({ setting.CalculateDownscaledResolution(resolution), kFormats[static_cast<size_t>(Layout::Moment)] });

	//* history *//

	buffers_[static_cast<size_t>(Layout::History)].Create({ resolution, kFormats[static_cast<size_t>(Layout::History)] });

	for (size_t i = 0; i < kLayoutCount; ++i) {
		// nameの設定
		std::string name = "FScreenSpaceProbeBuffer | ";
		name += magic_enum::enum_name(static_cast<FScreenSpaceProbeBuffer::Layout>(i));
		buffers_[i].SetName(name);
	}
}
