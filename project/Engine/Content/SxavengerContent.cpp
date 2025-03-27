#include "SxavengerContent.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerContent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerContent::Init() {
	collection_ = std::make_unique<OffscreenTextureCollection>();

	skinningPipeline_ = std::make_unique<SkinningComputePipeline>();
	skinningPipeline_->Init();

	debugPrimitive_ = std::make_unique<DebugPrimitive>();
	debugPrimitive_->Init();
}

void SxavengerContent::Term() {
	debugPrimitive_.reset();
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

void SxavengerContent::ResetPrimtive() {
	debugPrimitive_->ResetPrimitive();
}

void SxavengerContent::PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	debugPrimitive_->PushLine(v1, v2, color);
}

void SxavengerContent::PushAxis(const Vector3f& center, float length) {
	debugPrimitive_->PushAxis(center, length);
}

void SxavengerContent::PushSphere(const Vector3f& center, float radius, const Color4f& color) {
	debugPrimitive_->PushSphere(center, radius, color);
}
