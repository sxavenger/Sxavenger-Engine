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

	setting_ = std::make_unique<FPostProcessSetting>();

	renderer_ = std::make_unique<FSceneRenderer>();
	renderer_->SetTextures(textures_.get());
	renderer_->SetScene(scene_.get());
	renderer_->SetPostProcessSetting(setting_.get());

	presenter_.Init();

	//* camera *//

	camera_ = std::make_unique<APivotCameraActor>();
	camera_->Init();

	renderer_->SetCamera(camera_.get());

	//* geometries *//

	model_ = std::make_unique<AModelActor>();
	model_->Init();
	model_->SetModel(SxavengerAsset::TryImport<AssetModel>("asset/models/room/room.obj"));
	model_->SetRenderWait(false);

	scene_->AddGeometry(model_.get());

	//* light *//

	light_ = std::make_unique<APointLightActor>();
	light_->Init();

	scene_->AddLight(light_.get());

	//* process *//

	lut_ = std::make_unique<FProcessLut>();
	lut_->Init();

	setting_->AddProcess(lut_.get());

	attribute_ = std::make_unique<AttributeComponent>();
	attribute_->SetName("lut");
	attribute_->SetToOutliner();
	attribute_->SetAttributeFunc([this]() {
		if (ImGui::TreeNode("red")) {
			lut_->GetParameter().r.SetImGuiCommand();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("green")) {
			lut_->GetParameter().g.SetImGuiCommand();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("blue")) {
			lut_->GetParameter().b.SetImGuiCommand();
			ImGui::TreePop();
		}
	});

	//* test *//

	p_ = std::make_unique<DxObject::ReflectionComputePipelineState>();
	p_->CreateBlob("asset/shaders/RayQuerySample.cs.hlsl");
	p_->ReflectionPipeline(SxavengerSystem::GetDxDevice());

	//* editors *//

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([this](RenderSceneEditor* editor) {
		editor->SetGameRenderer(renderer_.get());
	});
}

void BetaSystemGameLoop::TermSystem() {
	attribute_.reset();
}

void BetaSystemGameLoop::UpdateSystem() {

	if (SxavengerSystem::IsPressKey(KeyId::KEY_1)) {
		auto mouse = SxavengerSystem::GetInput()->GetMouseInput();

		if (mouse->IsPress(MouseId::MOUSE_MIDDLE)) {
			Vector2f delta = mouse->GetDeltaPosition();
			static const Vector2f kSensitivity = { 0.01f, 0.01f };

			camera_->GetParameter().angle += delta * kSensitivity;
			camera_->GetParameter().angle.x = std::fmod(camera_->GetParameter().angle.x, pi_v * 2.0f);
			camera_->GetParameter().angle.y = std::clamp(camera_->GetParameter().angle.y, -pi_v / 2.0f, pi_v / 2.0f);

		}

		if (mouse->IsPress(MouseId::MOUSE_RIGHT)) {
			Vector2f delta = mouse->GetDeltaPosition();
			static const Vector2f kSensitivity = { 0.01f, 0.01f };

			Vector3f right = RotateVector({ 1.0f, 0.0f, 0.0f }, camera_->GetTransform().rotate);
			Vector3f up = RotateVector({ 0.0f, 1.0f, 0.0f }, camera_->GetTransform().rotate);

			camera_->GetParameter().point -= right * delta.x * kSensitivity.x;
			camera_->GetParameter().point += up * delta.y * kSensitivity.y;
		}

		camera_->GetParameter().distance = std::max(camera_->GetParameter().distance - mouse->GetDeltaWheel(), 0.0f);
		camera_->UpdateView();
	}

}

void BetaSystemGameLoop::DrawSystem() {

	renderer_->Render(SxavengerSystem::GetMainThreadContext());

	sEditorEngine->ExecuteEditorFunction<RenderSceneEditor>([](RenderSceneEditor* editor) {
		editor->Draw();
	});

	main_->BeginRendering();
	main_->ClearWindow();

	presenter_.Present(SxavengerSystem::GetMainThreadContext(), textures_->GetSize(), textures_->GetGBuffer(FSceneTextures::GBufferLayout::Result)->GetGPUHandleSRV());

	SxavengerSystem::RenderImGui();

	main_->EndRendering();
}
