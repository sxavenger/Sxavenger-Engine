#include "PrototypeGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>
#include <Engine/Asset/SxavengerAsset.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PrototypeGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrototypeGameLoop::Init(GameLoop::Context* context) {
	context->SetState(GameLoop::State::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetState(GameLoop::State::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetState(GameLoop::State::Update, std::nullopt, [this]() {
		if (auto developer = sEditorEngine->TryGetEditor<EngineDeveloperEditor>()) {
			if (!developer->IsProcessRequired()) {
				return;
			}
		}

		UpdateSystem();
	});
	context->SetState(GameLoop::State::Draw, std::nullopt, [this]() { DrawSystem(); });

	context->SetCondition(
		[this]() { return !SxavengerSystem::ProcessMessage(); }
	);
}

void PrototypeGameLoop::Term() {
}

void PrototypeGameLoop::InitSystem() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"prototype window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });

	// vvv render system vvv //

	presenter_.Init();

	textures_ = std::make_unique<FSceneTextures>();
	textures_->Create(main_->GetSize());

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());

	scene_ = std::make_unique<FScene>();
	renderer_->SetScene(scene_.get());

	// vvv actors vvv //

	camera_ = std::make_unique<ACineCameraActor>();
	camera_->Init();

	renderer_->SetCamera(camera_.get());

	light_ = std::make_unique<ADirectionalLightActor>();
	light_->Init();

	scene_->AddLight(light_.get());

	// vvv prototype vvv //

	border_ = std::make_unique<Collider>();
	border_->SetColliderBoundingAABB({ min_, max_ });
	border_->SetToCollection();

	model_ = std::make_unique<AModelActor>();
	model_->Init();
	model_->SetModel(SxavengerAsset::TryImport<AssetModel>("assets/models/primitive/cube.obj"));

	scene_->AddGeometry(model_.get());

	//* editor *//

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([this](RenderSceneEditor* editor) {
		editor->SetGameRenderer(renderer_.get());
	});

}

void PrototypeGameLoop::TermSystem() {
}

void PrototypeGameLoop::UpdateSystem() {

	// move

	//* clamp *//

	Vector3f& translate = model_->GetTransform().translate;

	for (size_t i = 0; i < 3; ++i) {
		if (translate[i] < min_[i]) {
			translate[i] = max_[i] + (translate[i] - min_[i]);

		} else if (translate[i] > max_[i]) {
			translate[i] = min_[i] + (translate[i] - max_[i]);
		}
	}

	model_->TransformComponent::UpdateMatrix();
}

void PrototypeGameLoop::DrawSystem() {

	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([](RenderSceneEditor* editor) {
		editor->Render();
	});

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_.Present(SxavengerSystem::GetMainThreadContext(), main_->GetSize(), textures_->GetGBuffer(FSceneTextures::GBufferLayout::Main)->GetGPUHandleSRV());

	SxavengerSystem::RenderImGui();

	main_->EndRendering();

}
