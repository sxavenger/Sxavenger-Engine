#include "SxavengerGame.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	std::unique_ptr<ModelManager> sModelManager         = nullptr;
	std::unique_ptr<DebugPrimitive> sDebugPrimitive     = nullptr;
	std::unique_ptr<ColliderManager> sColliderManager   = nullptr;
	std::unique_ptr<SpriteCommon> sSpriteCommon         = nullptr;
	std::unique_ptr<SkinningPipeline> sSkinningPipeline = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerGame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerGameEngine::Init() {
	sModelManager = std::make_unique<ModelManager>();
	sModelManager->Init();

	sDebugPrimitive = std::make_unique<DebugPrimitive>();
	sDebugPrimitive->Init();

	sColliderManager = std::make_unique<ColliderManager>();
	sColliderManager->Init();

	sSpriteCommon = std::make_unique<SpriteCommon>();
	sSpriteCommon->Init();

	sSkinningPipeline = std::make_unique<SkinningPipeline>();
	sSkinningPipeline->Init();
}

void SxavengerGameEngine::Term() {
	sSkinningPipeline->Term();
	sSkinningPipeline.reset();

	sSpriteCommon->Term();
	sSpriteCommon.reset();

	sColliderManager->Term();
	sColliderManager.reset();

	sDebugPrimitive->Term();
	sDebugPrimitive.reset();

	sModelManager->Term();
	sModelManager.reset();
}

void SxavengerGameEngine::Reset() {
	sDebugPrimitive->ResetPrimitive();
	sSpriteCommon->Reset();
}

Model* SxavengerGameEngine::LoadModel(const std::string& directoryPath, const std::string& filename, bool smooth) {
	return sModelManager->Load(directoryPath, filename, smooth);
}

void SxavengerGameEngine::DeleteModel(const std::string& directoryPath, const std::string& filename) {
	sModelManager->Delete(directoryPath, filename);
}

ModelManager* SxavengerGameEngine::GetModelManager() {
	return sModelManager.get();
}

void SxavengerGameEngine::DrawToScene(const Camera3D* camera) {
	sDebugPrimitive->DrawToScene(camera);
}

void SxavengerGameEngine::ResetPrimitive() {
	sDebugPrimitive->ResetPrimitive();
}

void SxavengerGameEngine::DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	sDebugPrimitive->DrawLine(v1, v2, color);
}

void SxavengerGameEngine::DrawAxis(const Vector3f& center, float length) {
	sDebugPrimitive->DrawAxis(center, length);
}

DebugPrimitive* SxavengerGameEngine::GetDebugPrimitive() {
	return sDebugPrimitive.get();
}

void SxavengerGameEngine::SetCollider(Collider* collider) {
	sColliderManager->SetCollider(collider);
}

void SxavengerGameEngine::EraseCollider(Collider* collider) {
	sColliderManager->EraseCollider(collider);
}

void SxavengerGameEngine::ClearColliders() {
	sColliderManager->ClearColliders();
}

void SxavengerGameEngine::UpdateColliders() {
	sColliderManager->Update();
}

void SxavengerGameEngine::DrawColliders() {
	sColliderManager->DrawColliders();
}

ColliderManager* SxavengerGameEngine::GetColliderManager() {
	return sColliderManager.get();
}

void SxavengerGameEngine::DrawSprite(const Vector2f& pos, const Vector2f& size, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color) {
	sSpriteCommon->DrawSprite(pos, size, handle, color);
}

void SxavengerGameEngine::DrawSprite(const Vector2f& pos, const Vector2f& size, float rotate, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color) {
	sSpriteCommon->DrawSprite(pos, size, rotate, handle, color);
}

void SxavengerGameEngine::DrawSpriteClip(const Vector2f& pos, const Vector2f& size, const Vector2f& leftTopUV, const Vector2f& sizeUV, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color) {
	sSpriteCommon->DrawSpriteClip(pos, size, leftTopUV, sizeUV, handle, color);
}

SpriteCommon* SxavengerGameEngine::GetSpriteCommon() {
	return sSpriteCommon.get();
}

void SxavengerGameEngine::SetSkinningPipeline() {
	sSkinningPipeline->SetPipeline();
}

SkinningPipeline* SxavengerGameEngine::GetSkinningPipeline() {
	return sSkinningPipeline.get();
}
