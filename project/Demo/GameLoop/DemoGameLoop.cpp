//[DemoGameLoop.cpp]
#include "DemoGameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Editor/EditorEngine.h>
#include <Engine/Editor/Editors/DevelopEditor.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Component/Components/Collider/ColliderComponent.h>
#include <Engine/Component/Components/Collider/CollisionManager.h>
#include <Engine/Component/Components/SpriteRenderer/SpriteRendererComponent.h>
#include <Engine/Component/ComponentHelper.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoGameLoop::Init(GameLoop::Context* context) {
	context->SetProcess(GameLoop::Process::Init, std::nullopt, [this]() { InitGame(); });
	context->SetProcess(GameLoop::Process::Term, std::nullopt, [this]() { TermGame(); });
	context->SetProcess(GameLoop::Process::Update, std::nullopt, [this]() {
		if (auto developer = sEditorEngine->TryGetEditor<DevelopEditor>()) {
			if (!developer->IsProcessRequired()) {
				return;
			}
		}

		UpdateGame();
	});
	context->SetProcess(GameLoop::Process::Render, std::nullopt, [this]() { DrawGame(); });

	context->SetCondition([this]() { return !SxavengerSystem::ProcessMessage(); });
}

void DemoGameLoop::Term() {
}

void DemoGameLoop::InitGame() {

	main_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, L"Sxavenger Engine Demo").lock();
	main_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

#ifdef _DEVELOPMENT
	editor_ = SxavengerSystem::TryCreateSubWindow(kMainWindowSize, L"Editor Window").lock();
	editor_->SetIcon("packages/icon/SxavengerEngineSubIcon.ico", { 32, 32 });
#endif

	stage_ = std::make_unique<Stage>();
	stage_->Load();
	stage_->Awake();
	stage_->Start();

	player_ = std::make_unique<Player>();
	player_->Load();
	player_->Awake();
	player_->Start();

	smoke_ = std::make_unique<SmokeParticle>();
	smoke_->Load();
	smoke_->Awake();
	smoke_->Start();

	rain_ = std::make_unique<RainParticle>();
	rain_->Load();
	rain_->Awake();
	rain_->Start();

	sampleLight_ = ComponentHelper::CreateDirectionalLightMonoBehaviour();
	sampleLight_->GetComponent<TransformComponent>()->rotate *= Quaternion::ToQuaternion({ kPi / 4.0f, 0.0f, 0.0f });

	//AssetObserver<AssetTexture> env = SxavengerAsset::TryImport<AssetTexture>("assets/textures/EnvHDR.dds");
	//environmentMap_.Create(env.WaitGet()->GetSize());
	//environmentMap_.Dispatch(SxavengerSystem::GetMainThreadContext(), env.WaitGet()->GetGPUHandleSRV());

	SxavengerSystem::ExecuteAllAllocator();

	//skylight_ = ComponentHelper::CreateMonoBehaviour();
	//skylight_->SetName("sky light");
	//skylight_->AddComponent<SkyLightComponent>();
	//skylight_->GetComponent<SkyLightComponent>()->GetDiffuseParameter().SetTexture(environmentMap_.GetIrradianceIndex());
	//skylight_->GetComponent<SkyLightComponent>()->GetSpecularParameter().SetTexture(environmentMap_.GetRadianceIndex(), environmentMap_.GetRadianceMiplevel());

}

void DemoGameLoop::TermGame() {
}

void DemoGameLoop::UpdateGame() {

	//-----------------------------------------------------------------------------------------
	// GameLogic Update
	//-----------------------------------------------------------------------------------------

	player_->Update();

	smoke_->Update();
	rain_->Update();

	//-----------------------------------------------------------------------------------------
	// SystemUpdate...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateTransform();
	// todo: engine側のgameloopに移動.

	sCollisionManager->CheckCollision();
	// todo: engine側のgameloopに移動.

	//-----------------------------------------------------------------------------------------
	// LateUpdate
	//-----------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------------------------
	// final Update...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateSkinning();
	// todo: engine側のgameloopに移動.

	ComponentHelper::UpdateParticle();

}

void DemoGameLoop::DrawGame() {

	FMainRender::GetInstance()->Render(SxavengerSystem::GetMainThreadContext());

#ifdef _DEVELOPMENT
	editor_->BeginRendering();
	editor_->ClearWindow();

	editor_->EndRendering();
#endif

	main_->BeginRendering();
	main_->ClearWindow();


	FMainRender::GetInstance()->PresentMain(SxavengerSystem::GetMainThreadContext());
	SxavengerSystem::RenderImGui();
	
	main_->EndRendering();
}
