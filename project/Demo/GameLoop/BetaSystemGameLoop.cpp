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

	scene_ = std::make_unique<FScene>();

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());
	renderer_->SetScene(scene_.get());

	presenter_.Init();

	AssetObserver<AssetModel> observer = SxavengerAsset::TryImport<AssetModel>("assets/models/primitive/teapot.obj");
	mesh_ = observer.WaitGet()->CreateMonoBehavior("teapot");

	/*AssetObserver<AssetModel> observer = SxavengerAsset::TryImport<AssetModel>("assets/models/chessboard/chessboard.gltf");
	mesh_ = observer.WaitGet()->CreateMonoBehavior("chessboard");*/

	camera_ = std::make_unique<MonoBehaviour>();
	camera_->AddComponent<TransformComponent>();
	auto camera = camera_->AddComponent<CameraComponent>();
	camera->Init();
	camera->SetTag(CameraComponent::Tag::GameCamera);

	light_ = std::make_unique<MonoBehaviour>();
	auto transform = light_->AddComponent<TransformComponent>();
	transform->GetTransform().rotate = AxisAngle({ 1.0f, 0.0f, 0.0f }, pi_v / 2.0f);
	transform->UpdateMatrix();
	auto light = light_->AddComponent<DirectionalLightComponent>();
	light->Init();

	sEditorEngine->ExecuteEditorFunction<OutlinerEditor>([&](OutlinerEditor* editor) {
		editor->SetBehaviour(camera_.get());
	});
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {

}

void BetaSystemGameLoop::DrawSystem() {

	scene_->SetupTopLevelAS(SxavengerSystem::GetMainThreadContext());

	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_.Present(SxavengerSystem::GetMainThreadContext(), main_->GetSize(), textures_->GetGBuffer(FRenderTargetTextures::GBufferLayout::Main)->GetGPUHandleSRV());
	 
	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
