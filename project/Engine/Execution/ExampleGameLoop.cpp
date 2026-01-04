#include "ExampleGameLoop.h"
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
// ExampleGameLoop class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExampleGameLoop::Init(Execution::Context* context) {
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

void ExampleGameLoop::Term() {
}

void ExampleGameLoop::InitSystem() {

	main_ = System::CreateMainWindow(
		Configuration::GetConfig().resolution,
		L"[Sxavenger Engine / Sxx Engine] Example Window"
	).lock();

	atmosphere_ = std::make_unique<GameObject>();
	(*atmosphere_)->AddComponent<TransformComponent>();
	(*atmosphere_)->AddComponent<SkyAtmosphereComponent>();
	(*atmosphere_)->AddComponent<SkyLightComponent>();

	camera_ = std::make_unique<ControllableCameraActor>();


	SxxEngine::ContentObserver<ContentModel> observer
		= SxxEngine::sContentStorage->Import<ContentModel>("assets/models/sponza/sponza.gltf");

	auto content = observer.WaitGet();
	
}

void ExampleGameLoop::TermSystem() {
}

void ExampleGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------

	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->UpdateTransmittance(System::GetDirectQueueContext());
	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->UpdateMultipleScattering(System::GetDirectQueueContext());
	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->UpdateSkyCube(System::GetDirectQueueContext());

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

	ComponentHelper::UpdateSkinning();
	// todo: engine側のgameloopに移動.

	ComponentHelper::UpdateAudio3d();
}

void ExampleGameLoop::RenderSystem() {

	FMainRender::GetInstance()->Render(System::GetDirectQueueContext(), main_.get());

	main_->BeginRenderWindow(System::GetDirectQueueContext());
	main_->ClearWindow(System::GetDirectQueueContext());

	FMainRender::GetInstance()->PresentMain(System::GetDirectQueueContext());
	System::RenderImGui();

	main_->EndRenderWindow(System::GetDirectQueueContext());

}
