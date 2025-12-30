#include "DemoGameLoop.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Configuration/Configuration.h>
#include <Engine/System/System.h>
#include <Engine/Components/Component/Transform/TransformComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyAtmosphereComponent.h>
#include <Engine/Components/Component/Light/Environment/SkyLightComponent.h>
#include <Engine/Components/Component/ComponentHelper.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Editors/EditorEngine.h>
#include <Engine/Editors/Editor/DevelopEditor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DemoGameLoop::Init(Execution::Context* context) {
	context->SetProcess(Execution::Process::Init, std::nullopt, [this]() { InitSystem(); });
	context->SetProcess(Execution::Process::Term, std::nullopt, [this]() { TermSystem(); });
	context->SetProcess(Execution::Process::Update, std::nullopt, [this]() {
		if (auto develop = sEditorEngine->GetEditor<DevelopEditor>()) {
			if (!develop->IsProcessRequired()) {
				return;
			}
		}

		UpdateSystem();
	});
	context->SetProcess(Execution::Process::Render, std::nullopt, [this]() { RenderSystem(); });

	context->SetCondition(
		[this]() { return !System::ProcessMessage(); }
	);
}

void DemoGameLoop::Term() {
}

void DemoGameLoop::InitSystem() {

	main_ = System::CreateMainWindow(
		Configuration::GetConfig().resolution,
		L"[Sxavenger Engine / Sxx Engine] Demo Window"
	).lock();

	atmosphere_ = std::make_unique<GameObject>();
	(*atmosphere_)->AddComponent<TransformComponent>();
	(*atmosphere_)->AddComponent<SkyAtmosphereComponent>();
	(*atmosphere_)->AddComponent<SkyLightComponent>();

	//* ptr作成 / load *//

	player_ = std::make_unique<Player>();
	player_->Load();

	playerHandle_ = std::make_unique<PlayerHandle>();

	camera_       = std::make_unique<PerspectiveCamera>();
	cameraHandle_ = std::make_unique<CameraHandle>();

	//* awake *//

	player_->Awake();
	camera_->Awake();
	(*player_)->AddChild(camera_->GetAddress());
}

void DemoGameLoop::TermSystem() {
}

void DemoGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------

	playerHandle_->Update();

	switch (playerHandle_->GetMoveState()) {
		case PlayerHandle::MoveState::Walk:
			player_->SetWalk(camera_->CalculateMoveFront(playerHandle_->GetMoveVector()));
			break;

		case PlayerHandle::MoveState::Dash:
			player_->SetDash(camera_->CalculateMoveFront(playerHandle_->GetMoveVector()));
			break;

		default:
			player_->SetIdle();
			break;
	}

	player_->Update();

	cameraHandle_->Update();

	camera_->SetAround(cameraHandle_->GetDirection());
	camera_->Update();

	//-----------------------------------------------------------------------------------------
	// SystemUpdate...?
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateTransform();
	// todo: engine側のgameloopに移動.

	//-----------------------------------------------------------------------------------------
	// LateUpdate
	//-----------------------------------------------------------------------------------------

	//-----------------------------------------------------------------------------------------
	// final Update...?
	//-----------------------------------------------------------------------------------------

	//!< TODO: ComponentHelperに移動予定
	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->UpdateTransmittance(System::GetDirectQueueContext());
	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->UpdateMultipleScattering(System::GetDirectQueueContext());
	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->UpdateSkyCube(System::GetDirectQueueContext());

	ComponentHelper::UpdateSkinning();
	// todo: engine側のgameloopに移動.

	ComponentHelper::UpdateAudio3d();
}

void DemoGameLoop::RenderSystem() {

	FMainRender::GetInstance()->Render(System::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(System::GetDirectQueueContext());
	main_->ClearWindow(System::GetDirectQueueContext());

	FMainRender::GetInstance()->PresentMain(System::GetDirectQueueContext());
	System::RenderImGui();

	main_->EndRenderWindow(System::GetDirectQueueContext());

}
