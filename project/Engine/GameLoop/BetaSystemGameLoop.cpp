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


#include "Engine/!Render/FRenderCore.h"
#include "Engine/System/Config/SxavengerDirectory.h"

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

	FRenderCore::GetInstance()->Init();

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"beta system window").lock();
	main_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });
	textures_ = std::make_unique<FSceneTextures>();
	textures_->Create(main_->GetSize());

	scene_ = std::make_unique<FScene>();

	ambientProcess_ = std::make_unique<FAmbientProcessSetting>();
	postProcess_ = std::make_unique<FPostProcessSetting>();

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());
	renderer_->SetScene(scene_.get());
	renderer_->SetAmbientSetting(ambientProcess_.get());
	renderer_->SetPostProcessSetting(postProcess_.get());

	presenter_.Init();

	//* camera *//

	camera_ = std::make_unique<ACineCameraActor>();
	camera_->Init();

	renderer_->SetCamera(camera_.get());

	//* geometries *//

	model_ = std::make_unique<AModelActor>();
	model_->Init();
	model_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/models/room/room.obj"));
	model_->SetRenderWait(false);

	scene_->AddGeometry(model_.get());

	model1_ = std::make_unique<AModelActor>();
	model1_->Init();
	model1_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/models/primitive/teapot.obj"));
	model1_->SetTransparency(AGeometryActor::Transparency::Opaque);

	scene_->AddGeometry(model1_.get());

	//* light *//

	light_ = std::make_unique<APointLightActor>();
	light_->Init();

	scene_->AddLight(light_.get());

	//* process *//

	processDoF_ = std::make_unique<FPostProcessDoF>();
	processDoF_->Init();
	//postProcess_->AddProcess(processDoF_.get());

	doFComponent_ = std::make_unique<AttributeComponent>();
	//doFComponent_->SetToOutliner();
	doFComponent_->SetAttributeFunc([this]() { processDoF_->SetImGuiCommand(); });

	processNLAO_ = std::make_unique<FAmbientProcessNLAO>();
	processNLAO_->Init();
	ambientProcess_->AddProcess(processNLAO_.get());

	nlaoComponent_ = std::make_unique<AttributeComponent>();
	nlaoComponent_->SetToOutliner();
	nlaoComponent_->SetAttributeFunc([this]() { processNLAO_->SetImGuiCommand(); });

	/*processBloom_ = std::make_unique<FPostProcessBloom>();
	processBloom_->Init();
	postProcess_->AddProcess(processBloom_.get());

	bloomComponent_ = std::make_unique<AttributeComponent>();
	bloomComponent_->SetName("Bloom");
	bloomComponent_->SetToOutliner();
	bloomComponent_->SetAttributeFunc([this]() { processBloom_->SetImGuiCommand(); });*/

	processVignette_ = std::make_unique<FPostProcessVignette>();
	processVignette_->Init();
	postProcess_->AddProcess(processVignette_.get());

	vignetteComponent_ = std::make_unique<AttributeComponent>();
	vignetteComponent_->SetName("Vignette");
	vignetteComponent_->SetToOutliner();
	vignetteComponent_->SetAttributeFunc([this]() { processVignette_->SetImGuiCommand(); });

	//* editors *//

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([this](RenderSceneEditor* editor) {
		editor->SetGameRenderer(renderer_.get());
	});

	
}

void BetaSystemGameLoop::TermSystem() {
	nlaoComponent_.reset();
}

void BetaSystemGameLoop::UpdateSystem() {
	processDoF_->SetFocus(model1_->GetPosition(), camera_.get());

	if (SxavengerSystem::IsTriggerKey(KeyId::KEY_SPACE)) {
		if (main_->GetMode() == Window::Mode::Borderless) {
			main_->SetWindowMode(Window::Mode::Window);

		} else {
			main_->SetWindowMode(Window::Mode::Borderless);
		}
	}
}

void BetaSystemGameLoop::DrawSystem() {

	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([](RenderSceneEditor* editor) {
		editor->Draw();
	});

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_.Present(SxavengerSystem::GetMainThreadContext(), textures_->GetSize(), textures_->GetGBuffer(FSceneTextures::GBufferLayout::Main)->GetGPUHandleSRV());

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
