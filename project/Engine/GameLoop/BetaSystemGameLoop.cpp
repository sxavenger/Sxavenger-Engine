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

	assetA_ = SxavengerAsset::TryImport<AssetModel>("asset/model/chessBoard/chessboard.gltf");

	assetB_ = SxavengerAsset::TryImport<AssetModel>("asset/model/human/idle.gltf");
	assetC_ = SxavengerAsset::TryImport<AssetAnimator>("asset/model/human/idle.gltf");
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {
	sDeveloperGui->RenderMainMenu();
}

void BetaSystemGameLoop::DrawSystem() {
	main_->BeginRendering();
	main_->ClearWindow();

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
