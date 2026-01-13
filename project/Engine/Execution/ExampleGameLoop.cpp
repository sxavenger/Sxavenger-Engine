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
#include <Engine/Components/Component/Transform/RectTransformComponent.h>
#include <Engine/Components/Component/CanvasRenderer/TextRendererComponent.h>
#include <Engine/Components/Component/ComponentHelper.h>
#include <Engine/Components/Entity/BehaviourHelper.h>
#include <Engine/Components/Entity/EntityBehaviourStorage.h>
#include <Engine/Render/FMainRender.h>
#include <Engine/Editors/EditorEngine.h>
#include <Engine/Editors/Editor/DevelopEditor.h>

//* lib
#include <Lib/Adapter/Random/Random.h>

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

	context->SetCondition([]() { return !System::ProcessMessage(); });             //!< windowが閉じられた場合
	context->SetCondition([]() { return System::IsPressKey(KeyId::KEY_ESCAPE); }); //!< ESCキーが押された
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
	(*atmosphere_)->AddComponent<DirectionalLightComponent>();

	(*atmosphere_)->GetComponent<TransformComponent>()->rotate = Quaternion::AxisAngle(Vector3f{ 1.0f, 0.0f, 0.0f }.Normalize(), kPi / 2.0f);

	camera_ = std::make_unique<ControllableCameraActor>();

	performance_ = std::make_unique<PerformanceActor>();
	performance_->SetPosition({ 1190.0f, 0.0f });

	{
		demoText_ = std::make_unique<GameObject>();
		(*demoText_)->SetName("demo text");

		auto& transform = (*demoText_)->AddComponent<RectTransformComponent>()->GetTransform();
		transform.scale     = { 400.0f, 50.0f };
		transform.pivot     = { 0.0f, 0.0f };
		transform.translate = { 16.0f, 0.0f };

		auto text = (*demoText_)->AddComponent<TextRendererComponent>();
		text->SetFont(sContentStorage->Import<ContentFont>("packages/font/MPLUSRounded1c-Regular.ttf")->GetId());
		text->SetSize(32.0f);
		text->SetText(L"Sxavenger Engine Demo");
	}

	{
		instructionText_ = std::make_unique<GameObject>();
		(*instructionText_)->SetName("text");

		auto& transform = (*instructionText_)->AddComponent<RectTransformComponent>()->GetTransform();
		transform.scale     = { 400.0f, 200.0f };
		transform.pivot     = { 0.0f, 1.0f };
		transform.translate = { 16.0f, 760.0f };

		auto text = (*instructionText_)->AddComponent<TextRendererComponent>();
		text->SetFont(sContentStorage->Import<ContentFont>("packages/font/MPLUSRounded1c-Regular.ttf")->GetId());
		text->SetSize(20.0f);

		std::wstring t = L"";
		t += L"[みぎクリック] : してんいどう\n";
		t += L"[P]          : パストレーシングモード\n";
		t += L"[<][>]       : たいようのいどう\n";
		t += L"[ESC]        : ゲームしゅうりょう\n";

		text->SetText(t);
	}

	{
		json data;
		if (JsonHandler::LoadFromJson("assets/scene/sponza.scene", data)) {
			sEntityBehaviourStorage->InputJson(data);
		}
	}
}

void ExampleGameLoop::TermSystem() {
}

void ExampleGameLoop::UpdateSystem() {

	//-----------------------------------------------------------------------------------------
	// Update
	//-----------------------------------------------------------------------------------------

	camera_->Update();

	auto keyboard = System::GetKeyboardInput();

	if (keyboard->IsTrigger(KeyId::KEY_P)) {
		auto& config = FMainRender::GetInstance()->GetConfig();
		config.option.Inverse(FBaseRenderPass::Config::Option::IndirectLighting);
	}


	if (keyboard->IsPress(KeyId::KEY_LEFT)) {
		(*atmosphere_)->GetComponent<TransformComponent>()->rotate *= Quaternion::AxisAngle(Vector3f{ 1.0f, 1.0f, 0.0f }.Normalize(), 0.01f);
	}

	if (keyboard->IsPress(KeyId::KEY_RIGHT)) {
		(*atmosphere_)->GetComponent<TransformComponent>()->rotate *= Quaternion::AxisAngle(Vector3f{ 1.0f, 1.0f, 0.0f }.Normalize(), -0.01f);
	}

	performance_->Update();

	//-----------------------------------------------------------------------------------------
	// SystemUpdate
	//-----------------------------------------------------------------------------------------

	ComponentHelper::UpdateTransform();
	// todo: engine側のgameloopに移動.

	//-----------------------------------------------------------------------------------------
	// LateUpdate
	//-----------------------------------------------------------------------------------------

	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->UpdateTransmittance(System::GetDirectQueueContext());
	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->UpdateMultipleScattering(System::GetDirectQueueContext());
	(*atmosphere_)->GetComponent<SkyAtmosphereComponent>()->UpdateSkyCube(System::GetDirectQueueContext());
	// todo: componentのupdateとしてまとめる.

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
