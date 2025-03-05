#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Editor/Editors/OutlinerEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->TryGetEditor<DevelopEditor>()) {
			if (!develop->IsProcessRequired()) {
				return;
			}
		}

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

	AssetObserver<AssetModel> observerA = SxavengerAsset::TryImport<AssetModel>("assets/models/chessboard/chessboard.gltf");
	//behaviourA_ = observerA.WaitGet()->CreateMonoBehavior("teapot");

	for (size_t i = 0; i < 1; i++) {
		behaviourA_[i] = observerA.WaitGet()->CreateMonoBehavior("teapot");

		for (auto& child : behaviourA_[i]->GetChildren()) {
			if (auto component = std::get_if<std::unique_ptr<MonoBehaviour>>(&child)) {
				auto transform = (*component)->GetComponent<TransformComponent>();
				transform->GetTransform().translate = { static_cast<float>(i) * 0.18f, static_cast<float>(i) * 0.18f, static_cast<float>(i) };
				transform->GetTransform().scale     = { 1.0f, 1.0f, 1.0f };
				transform->UpdateMatrix();
			}
		}
	}

	camera_ = std::make_unique<MonoBehaviour>();
	camera_->AddComponent<TransformComponent>();
	auto camera = camera_->AddComponent<CameraComponent>();
	camera->Init();
	camera->SetTag(CameraComponent::Tag::GameCamera);

	if (auto outliner = sEditorEngine->TryGetEditor<OutlinerEditor>()) {
		outliner->SetBehaviour(camera_.get());
	}
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
