#include "SxavengerGame.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	std::unique_ptr<ModelManager> sModelManager     = nullptr;
	std::unique_ptr<DebugPrimitive> sDebugPrimitive = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerGame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerGameEngine::Init() {

	sModelManager = std::make_unique<ModelManager>();
	sModelManager->Init();

	sDebugPrimitive = std::make_unique<DebugPrimitive>();
	sDebugPrimitive->Init();
}

void SxavengerGameEngine::Term() {
	sDebugPrimitive->Term();
	sDebugPrimitive.reset();

	sModelManager->Term();
	sModelManager.reset();
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

void SxavengerGameEngine::CountPrimitiveBufferOffset() {
	sDebugPrimitive->CountBufferOffset();
}

void SxavengerGameEngine::ResetPrimitive() {
	sDebugPrimitive->ResetPrimitive();
}

void SxavengerGameEngine::DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	sDebugPrimitive->DrawLine(v1, v2, color);
}

DebugPrimitive* SxavengerGameEngine::GetDebugPrimitive() {
	return sDebugPrimitive.get();
}
