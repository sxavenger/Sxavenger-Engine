#include "GuizmoScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/Performance/Performance.h>
#include <Engine/Console/Console.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Asset/SxavengerAsset.h>

//* lib
#include <Lib/Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GuizmoScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GuizmoScene::Run() {

	//-----------------------------------------------------------------------------------------
	// 初期化処理
	//-----------------------------------------------------------------------------------------
	SystemInit();
	Init();
	SxavengerSystem::ExecuteAllAllocator();

	//-----------------------------------------------------------------------------------------
	// ゲームループ
	//-----------------------------------------------------------------------------------------
	while (SxavengerSystem::ProcessMessage()) {

		Performance::BeginFrame();
		SxavengerSystem::BeginImGuiFrame();

		//-----------------------------------------------------------------------------------------
		// 更新処理
		//-----------------------------------------------------------------------------------------

		SxavengerSystem::GetInput()->Update();

		Update();

		SxavengerSystem::EndImGuiFrame();
		SxavengerSystem::TransitionAllocator();

		//-----------------------------------------------------------------------------------------
		// 描画処理
		//-----------------------------------------------------------------------------------------

		Draw();
		DrawScreen();

		SxavengerSystem::PresentAllWindow();
		SxavengerSystem::ExecuteAllAllocator();
		//!< sub window delete時に死ぬ
		//!< shader hot reload 時に死ぬ

		Performance::EndFrame();

		if (SxavengerSystem::IsTriggerKey(KeyId::KEY_F4)) {
			break;
		}
	}

	//-----------------------------------------------------------------------------------------
	// 終了処理
	//-----------------------------------------------------------------------------------------

	Term();

}

void GuizmoScene::SystemInit() {


	mainWindow_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, kMainWindowTitle).lock();
	mainWindow_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	{
		std::unique_ptr<DxObject::ReflectionComputePipelineState> compute = std::make_unique<DxObject::ReflectionComputePipelineState>();
		compute->CreateBlob("common/white1x1.cs.hlsl");
		compute->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		std::shared_ptr<UnorderedTexture> white1x1 = SxavengerContent::TryCreateUnorderedTexture("white1x1.png", { 1, 1 });
		white1x1->TransitionBeginUnordered(SxavengerSystem::GetMainThreadContext());
		compute->SetPipeline(SxavengerSystem::GetCommandList());


		DxObject::BindBufferDesc bind = {};
		bind.SetHandle("gOutput", white1x1->GetGPUHandleUAV());

		compute->Dispatch(SxavengerSystem::GetCommandList(), 1, 1, 1);

		white1x1->TransitionEndUnordered(SxavengerSystem::GetMainThreadContext());
		SxavengerSystem::TransitionAllocator();
	}


}

void GuizmoScene::Init() {

	camera_ = std::make_unique<Camera3d>();
	camera_->Create();

	component_.CreateBuffer();
}

void GuizmoScene::Update() {

	Matrix4x4 m = component_.GetMatrix();

	ImGui::Begin("camera");

	camera_->SetImGuiCommand();
	camera_->UpdateMatrix();
	ImGui::End();

	ImGuizmo::SetRect(0, 0, static_cast<float>(kMainWindowSize.x), static_cast<float>(kMainWindowSize.y));

	ImGuizmo::Manipulate(
		reinterpret_cast<const float*>(camera_->GetView().m),
		reinterpret_cast<const float*>(camera_->GetProj().m),
		ImGuizmo::TRANSLATE,
		ImGuizmo::WORLD,
		reinterpret_cast<float*>(m.m)
	);

	EulerTransform t = {};

	ImGuizmo::DecomposeMatrixToComponents(
		reinterpret_cast<float*>(m.m),
		&t.translate.x,
		&t.rotate.x,
		&t.scale.x
	);

	component_.GetTransform().translate = t.translate;
	component_.GetTransform().rotate    = ToQuaternion(t.rotate);
	component_.GetTransform().scale     = t.scale;
	component_.UpdateMatrix();

	ImGuizmo::DrawGrid(
		reinterpret_cast<const float*>(camera_->GetView().m),
		reinterpret_cast<const float*>(camera_->GetProj().m),
		reinterpret_cast<const float*>(Matrix4x4::Identity().m),
		12.0f
	);
}

void GuizmoScene::Draw() {
}

void GuizmoScene::DrawScreen() {

	mainWindow_->BeginRendering();
	mainWindow_->ClearWindow();

	SxavengerSystem::RenderImGui();

	mainWindow_->EndRendering();
}

void GuizmoScene::Term() {
}
