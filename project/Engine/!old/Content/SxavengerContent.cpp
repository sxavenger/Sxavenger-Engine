#include "SxavengerContent.h"

//=========================================================================================
// static variables
//=========================================================================================

std::unique_ptr<OffscreenTextureCollection> SxavengerContent::collection_ = nullptr;
std::unique_ptr<SkinningComputePipeline> SxavengerContent::skinningPipeline_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerContent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerContent::Init() {
	collection_ = std::make_unique<OffscreenTextureCollection>();
	skinningPipeline_ = std::make_unique<SkinningComputePipeline>();
	skinningPipeline_->Init();
}

void SxavengerContent::Term() {
	skinningPipeline_.reset();
	collection_.reset();
}

void SxavengerContent::RegisterTexture(const std::string& name, std::unique_ptr<BaseOffscreenTexture>&& texture) {
	collection_->RegisterTexture(name, std::move(texture));
}

const D3D12_GPU_DESCRIPTOR_HANDLE& SxavengerContent::GetGPUHandleSRV(const std::string& name) {
	return collection_->GetGPUHandleSRV(name);
}

void SxavengerContent::SetSkinningPipeline(const DirectXThreadContext* context) {
	skinningPipeline_->SetPipeline(context);
}

void SxavengerContent::DispatchSkinning(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize) {
	skinningPipeline_->Dispatch(context, desc, vertexSize);
}
