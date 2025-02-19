#include "BetaSystemGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/EngineDeveloperEditor.h>
#include <Engine/Editor/Editors/RenderSceneEditor.h>
#include <Engine/Module/SxavengerModule.h>

#include <Lib/Sxl/Optional.h>
#include <Engine/System/Config/SxavengerConfig.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BetaSystemGameLoop::Init(GameLoop::Context* context) {
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

void BetaSystemGameLoop::Term() {
}

void BetaSystemGameLoop::InitSystem() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"sxavenger engine beta window").lock();
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

	model_ = std::make_unique<AModelActor>();
	model_->Init();
	model_->SetModel(SxavengerAsset::TryImport<AssetModel>("assets/models/primitive/triangle.obj"));
	model_->SetRenderWait(true);

	model2_ = std::make_unique<AModelActor>();
	model2_->Init();
	model2_->SetModel(SxavengerAsset::TryImport<AssetModel>("assets/models/bricks/bricks.obj"));

	static const size_t kInstanceCount = 4096;

	instance_ = std::make_unique<AModelInstanceActor>();
	instance_->Init(kInstanceCount);
	instance_->SetModel(SxavengerAsset::TryImport<AssetModel>("assets/models/primitive/triangle.obj"));

	for (uint32_t i = 0; i < kInstanceCount; ++i) {
		instance_->SetMatrix(i, Matrix::MakeTranslate({ i * 0.01f, 0.0f, i * 0.01f}));
	}

	//scene_->AddGeometry(model_.get());
	//scene_->AddGeometry(model2_.get());
	scene_->AddGeometry(instance_.get());

	light_ = std::make_unique<ADirectionalLightActor>();
	light_->Init();

	scene_->AddLight(light_.get());

	//* editor *//

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([this](RenderSceneEditor* editor) {
		editor->SetGameRenderer(renderer_.get());
	});
}

void BetaSystemGameLoop::TermSystem() {
}

void BetaSystemGameLoop::UpdateSystem() {
}

void BetaSystemGameLoop::DrawSystem() {

	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([](RenderSceneEditor* editor) {
		editor->Draw();
	});

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_.Present(SxavengerSystem::GetMainThreadContext(), main_->GetSize(), textures_->GetGBuffer(FSceneTextures::GBufferLayout::Main)->GetGPUHandleSRV());

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
