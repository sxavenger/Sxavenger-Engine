#include "FTransparentGBuffer.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* GBuffer
#include "FMainGBuffer.h"

//* engine
#include <Engine/System/Utility/Convert.h>
#include <Engine/System/System.h>

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FTransparentGBuffer::kLayoutCount> FTransparentGBuffer::kFormats = {
	FMainGBuffer::kColorFormat, //!< Accumulate
	DXGI_FORMAT_R16_UNORM,      //!< Revealage
};

//- Format
// Accumulation: [FMainGBuffer::ColorFormat] float4 accumulation
// Revealage:    [DXGI_FORMAT_R16_UNORM]     float transparent

////////////////////////////////////////////////////////////////////////////////////////////
// FTransparentGBuffer class
////////////////////////////////////////////////////////////////////////////////////////////

void FTransparentGBuffer::Init(const Vector2ui& size) {

	buffers_[static_cast<size_t>(Layout::Accumulate)] = std::make_unique<FBaseTexture>(size, GetFormat(Layout::Accumulate), FBaseTexture::Flag::All);
	buffers_[static_cast<size_t>(Layout::Revealage)]  = std::make_unique<FBaseTexture>(size, GetFormat(Layout::Revealage), FBaseTexture::Flag::All, kWhite4<float>);

	for (size_t i = 0; i < kLayoutCount; ++i) {
		// nameの設定
		std::string name = "FTransparentGBuffer | ";
		name += magic_enum::enum_name(static_cast<FTransparentGBuffer::Layout>(i));
		buffers_[i]->GetResource()->SetName(ToWString(name).c_str());
	}
}

FBaseTexture* FTransparentGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DXGI_FORMAT FTransparentGBuffer::GetFormat(Layout layout) {
	return kFormats[static_cast<size_t>(layout)];
}
