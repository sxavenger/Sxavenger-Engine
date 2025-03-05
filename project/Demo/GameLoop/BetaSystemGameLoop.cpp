#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Asset/Observer/AssetObserver.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		/*if (auto developer = sEditorEngine->TryGetEditor<EngineDeveloperEditor>()) {
			if (!developer->IsProcessRequired()) {
				return;
			}
		}*/

		UpdateSystem();
	});
	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() { DrawSystem(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void BetaSystemGameLoop::Term() {
}

void BetaSystemGameLoop::InitSystem() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"sxavenger engine beta window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });

	textures_ = std::make_unique<FRenderTargetTextures>();
	textures_->Create(main_->GetSize());

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());

	presenter_.Init();

	actor_.Init();
	renderer_->SetCamera(&actor_);

	AssetObserver<AssetModel> observerA = SxavengerAsset::TryImport<AssetModel>("assets/models/primitive/teapot.obj");
	behaviour_[0] = observerA.WaitGet()->CreateMonoBehavior("teapot");

	AssetObserver<AssetModel> observerB = SxavengerAsset::TryImport<AssetModel>("assets/models/chessboard/chessboard.gltf");
	behaviour_[1] = observerB.WaitGet()->CreateMonoBehavior("chessboard");

	for (auto& child : behaviour_[1]->GetChildren()) {
		if (auto component = std::get_if<std::unique_ptr<MonoBehaviour>>(&child)) {
			auto transform = (*component)->GetComponent<TransformComponent>();
			transform->GetTransform().translate = { 1.0f, -1.0f, 1.0f };
			transform->GetTransform().scale     = { 2.0f, 2.0f, 2.0f };
			transform->UpdateMatrix();
		}
	};
	
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

}

void BetaSystemGameLoop::DrawSystem() {

	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_.Present(SxavengerSystem::GetMainThreadContext(), main_->GetSize(), textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Albedo)->GetGPUHandleSRV());
	 
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
