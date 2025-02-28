#include "SxavengerModule.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	static std::unique_ptr<DebugPrimitive> sPrimitive                 = nullptr;
	static std::unique_ptr<ColliderCollection> sColliderCollection    = nullptr;
	static std::unique_ptr<SkinningComputePipeline> sSkinningPipeline = nullptr;
	static std::unique_ptr<AudioController> sAudioController          = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerModule class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerModule::Init() {
	sPrimitive = std::make_unique<DebugPrimitive>();
	sPrimitive->Init();

	sColliderCollection = std::make_unique<ColliderCollection>();
	sColliderCollection->Init();

	sSkinningPipeline = std::make_unique<SkinningComputePipeline>();
	sSkinningPipeline->Init();

	sAudioController = std::make_unique<AudioController>();
	sAudioController->Init();
}

void SxavengerModule::Term() {
	sSkinningPipeline.reset();
	sPrimitive.reset();
	sAudioController.reset();
}

void SxavengerModule::ResetPrimtive() {
	sPrimitive->ResetPrimitive();
	sColliderCollection->GetDrawer()->Clear();
}

void SxavengerModule::DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	sPrimitive->DrawLine(v1, v2, color);
}

void SxavengerModule::DrawAxis(const Vector3f& center, float length) {
	sPrimitive->DrawAxis(center, length);
}

void SxavengerModule::DrawGrid(const Vector3f& center, float size) {
	sPrimitive->DrawGrid(center, size);
}

void SxavengerModule::DrawBox(const Vector3f& min, const Vector3f& max, const Color4f& color) {
	sPrimitive->DrawBox(min, max, color);
}

DebugPrimitive* SxavengerModule::GetDebugPrimitive() {
	return sPrimitive.get();
}

void SxavengerModule::SetCollider(Collider* collider) {
	sColliderCollection->SetCollider(collider);
}

void SxavengerModule::EraseCollider(Collider* collider) {
	sColliderCollection->EraseCollider(collider);
}

void SxavengerModule::CheckCollision() {
	sColliderCollection->CheckCollision();
}

ColliderCollection* SxavengerModule::GetColliderCollection() {
	return sColliderCollection.get();
}

void SxavengerModule::SetSkinningPipeline(const DirectXThreadContext* context) {
	sSkinningPipeline->SetPipeline(context);
}

void SxavengerModule::DispatchSkinningPipeline(const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize) {
	sSkinningPipeline->Dispatch(context, desc, vertexSize);
}

std::unique_ptr<Audio> SxavengerModule::CreateAudio(const AudioBuffer* buffer, bool isLoop) {
	return sAudioController->CreateAudio(buffer, isLoop);
}

void SxavengerModule::PlayOneShot(const AudioBuffer* buffer, float volume) {
	sAudioController->PlayOneShot(buffer, volume);
}

AudioController* SxavengerModule::GetAudioController() {
	return sAudioController.get();
}
