#include "SxavengerGame.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace -anonymouse-
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	std::unique_ptr<ModelManager> sModelManager = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerGame class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerGameEngine::Init() {

	sModelManager = std::make_unique<ModelManager>();
	sModelManager->Init();
	
}

void SxavengerGameEngine::Term() {
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
