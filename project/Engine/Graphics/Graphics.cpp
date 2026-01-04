#include "Graphics.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// Graphics class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Graphics::Init() {
	skinning_ = std::make_unique<SkinningCompute>();
	skinning_->Init();

	collection_ = std::make_unique<OffscreenTextureCollection>();

	debugPrimitive_ = std::make_unique<DebugPrimitive>();
	debugPrimitive_->Init();
}

void Graphics::Term() {
	skinning_.reset();
	collection_.reset();
	debugPrimitive_.reset();
}

void Graphics::SetSkinningPipeline(const DirectXQueueContext* context) {
	skinning_->SetPipeline(context);
}

void Graphics::DispatchSkinning(const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize) {
	skinning_->Dispatch(context, desc, vertexSize);
}

void Graphics::RegisterTexture(const std::string& name, std::unique_ptr<BaseOffscreenTexture>&& texture) {
	collection_->RegisterTexture(name, std::move(texture));
}

const DxObject::Descriptor& Graphics::GetDescriptorSRV(const std::string& name) {
	return collection_->GetDescriptorSRV(name);
}

const D3D12_GPU_DESCRIPTOR_HANDLE& Graphics::GetGPUHandleSRV(const std::string& name) {
	return collection_->GetGPUHandleSRV(name);
}

void Graphics::ResetPrimtive() {
	debugPrimitive_->ResetPrimitive();
}

void Graphics::PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness) {
	debugPrimitive_->PushLine(v1, v2, color, thickness);
}

void Graphics::PushLineOverlay(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness) {
	debugPrimitive_->PushLineOverlay(v1, v2, color, thickness);
}

void Graphics::PushPoint(const Vector3f& v, const Color4f& color, float thickness) {
	debugPrimitive_->PushPoint(v, color, thickness);
}

void Graphics::PushPointOverlay(const Vector3f& v, const Color4f& color, float thickness) {
	debugPrimitive_->PushPointOverlay(v, color, thickness);
}

void Graphics::PushAxis(const Vector3f& center, float length) {
	debugPrimitive_->PushAxis(center, length);
}

void Graphics::PushBox(const Vector3f& min, const Vector3f& max, const Color4f& color, float thickness) {
	debugPrimitive_->PushBox(min, max, color, thickness);
}

void Graphics::PushBox(const Vector3f& position, const Vector3f& min, const Vector3f& max, const Color4f& color, float thickness) {
	debugPrimitive_->PushBox(position, min, max, color, thickness);
}

void Graphics::PushSphere(const Vector3f& center, float radius, const Color4f& color) {
	debugPrimitive_->PushSphere(center, radius, color);
}

void Graphics::PushGrid(const Matrix4x4& viewInv, const Matrix4x4& projInv, const Vector2f& size, float radius) {
	debugPrimitive_->PushGrid(viewInv, projInv, size, radius);
}

void Graphics::PushCone(const Vector3f& center, const Vector3f& direction, float radius, float angle, const Color4f& color) {
	debugPrimitive_->PushCone(center, direction, radius, angle, color);
}

