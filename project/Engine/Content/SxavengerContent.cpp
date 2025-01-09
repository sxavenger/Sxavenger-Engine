#include "SxavengerContent.h"

//=========================================================================================
// static variables
//=========================================================================================

std::unique_ptr<OffscreenTextureCollection> SxavengerContent::collection_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerContent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerContent::Init() {
	collection_ = std::make_unique<OffscreenTextureCollection>();
}

void SxavengerContent::Term() {
	collection_.reset();
}

void SxavengerContent::RegisterTexture(const std::string& name, std::unique_ptr<BaseOffscreenTexture>&& texture) {
	collection_->RegisterTexture(name, std::move(texture));
}

const D3D12_GPU_DESCRIPTOR_HANDLE& SxavengerContent::GetGPUHandleSRV(const std::string& name) {
	return collection_->GetGPUHandleSRV(name);
}
