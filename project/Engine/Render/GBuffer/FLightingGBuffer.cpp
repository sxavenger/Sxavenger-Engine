#include "FLightingGBuffer.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* GBuffer
#include "FMainGBuffer.h"

//=========================================================================================
// static const variables
//=========================================================================================

const std::array<DXGI_FORMAT, FLightingGBuffer::kLayoutCount_> FLightingGBuffer::kFormats_ = {
	FMainGBuffer::kColorFormat, //!< Direct
	FMainGBuffer::kColorFormat, //!< Indirect_Reservoir
	FMainGBuffer::kColorFormat, //!< Indirect_Denoiser
	FMainGBuffer::kColorFormat, //!< Indirect
	
};

////////////////////////////////////////////////////////////////////////////////////////////
// FLightingGBuffer class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FLightingGBuffer::Init(const Vector2ui& size) {

	//!< 最終Lighting結果格納用
	buffers_[static_cast<uint8_t>(Layout::Direct)]             = std::make_unique<FBaseTexture>(size, GetFormat(Layout::Direct),             FBaseTexture::Flag::All);
	buffers_[static_cast<uint8_t>(Layout::Indirect)]           = std::make_unique<FBaseTexture>(size, GetFormat(Layout::Indirect),           FBaseTexture::Flag::All);
	buffers_[static_cast<uint8_t>(Layout::Indirect_Reservoir)] = std::make_unique<FBaseTexture>(size, GetFormat(Layout::Indirect_Reservoir), FBaseTexture::Flag::All);
	buffers_[static_cast<uint8_t>(Layout::Indirect_Denoise)]   = std::make_unique<FBaseTexture>(size, GetFormat(Layout::Indirect_Denoise),   FBaseTexture::Flag::All);

	for (size_t i = 0; i < kLayoutCount_; ++i) {
		// nameの設定
		std::string name = "FLightingGBuffer | ";
		name += magic_enum::enum_name(static_cast<FLightingGBuffer::Layout>(i));
		buffers_[i]->GetResource()->SetName(ToWString(name).c_str());
	}

	for (size_t i = 0; i < magic_enum::enum_count<Reservoir>(); ++i) {
		reservoirs_[i] = std::make_unique<DxObject::UnorderedDimensionBuffer<FRenderCoreRestir::Reservoir<16>>>();
		reservoirs_[i]->Create(SxavengerSystem::GetDxDevice(), size.x * size.y);

		std::string name = "FLightingGBuffer | Reservoir | ";
		name += magic_enum::enum_name(static_cast<FLightingGBuffer::Reservoir>(i));
		reservoirs_[i]->GetResource()->SetName(ToWString(name).c_str());
	}

	moment_ = std::make_unique<DxObject::UnorderedDimensionBuffer<Vector2ui>>();
	moment_->Create(SxavengerSystem::GetDxDevice(), size.x * size.y);
	moment_->GetResource()->SetName(L"FLightingGBuffer | Moment");
}

FBaseTexture* FLightingGBuffer::GetGBuffer(Layout layout) const {
	return buffers_[static_cast<size_t>(layout)].get();
}

DxObject::UnorderedDimensionBuffer<FRenderCoreRestir::Reservoir<16>>* FLightingGBuffer::GetReservoir(Reservoir reservoir) const {
	return reservoirs_[static_cast<size_t>(reservoir)].get();
}

DXGI_FORMAT FLightingGBuffer::GetFormat(Layout layout) {
	return kFormats_[static_cast<size_t>(layout)];
}
