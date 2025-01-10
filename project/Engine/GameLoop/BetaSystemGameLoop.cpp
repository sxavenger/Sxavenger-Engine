#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Dev/DeveloperGui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetState(GameLoop::State::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() { UpdateSystem(); });
	context->SetState(GameLoop::State::Draw, std::nullopt, [this]() { DrawSystem(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void BetaSystemGameLoop::Term() {
}

void BetaSystemGameLoop::InitSystem() {
	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"beta system window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });

	//* render system *//

	scene_ = std::make_unique<FScene>();

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->CreateTextures(main_->GetSize());
	renderer_->SetScene(scene_.get());

	camera_ = std::make_unique<ACineCameraActor>();
	camera_->Init();
	renderer_->SetCamera(camera_.get());

	model_ = std::make_unique<AModelActor>();
	model_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/model/human/idle.gltf"));
	scene_->AddGeometry(model_.get());

}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {
	sDeveloperGui->RenderMainMenu();
}

void BetaSystemGameLoop::DrawSystem() {

	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
