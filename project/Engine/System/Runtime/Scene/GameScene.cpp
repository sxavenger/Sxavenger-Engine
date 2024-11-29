#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/Performance/Performance.h>
#include <Engine/Console/Console.h>
#include <Engine/Content/SxavengerContent.h>

//* lib
#include <Lib/Environment.h>

#include "Lib/Adapter/Json/JsonAdapter.h"


////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//-----------------------------------------------------------------------------------------
	// 初期化処理
	//-----------------------------------------------------------------------------------------
	sConsole->Init();
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
		sConsole->UpdateConsole();

		if (sConsole->IsUpdateRequired()) {
			Update();
		}

		SxavengerSystem::EndImGuiFrame();
		SxavengerSystem::TransitionAllocator();

		//-----------------------------------------------------------------------------------------
		// 描画処理
		//-----------------------------------------------------------------------------------------

		Draw();
		sConsole->DrawConsole();

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
	sConsole->Term();

}

void GameScene::Init() {
	mainWindow_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, kMainWindowTitle).lock();
	mainWindow_->SetIcon("resources/icon/SxavengerEngineIcon.ico", { 32, 32 });

	subWindow_ = SxavengerSystem::TryCreateSubWindow({ 400, 400 }, L"sub", ToColor4f(0x3A504BFF));

	sConsole->SetWindow(mainWindow_);

	transform_.CreateBuffer();
	transform_.GetTransform().scale = { 4.0f, 4.0f, 4.0f };
	transform_.UpdateMatrix();

	model_ = std::make_unique<Model>();
	//model_->AsyncLoad("resources/model/chessBoard", "chessBoard.gltf", Model::GetDefaultAssimpOption() | aiProcess_Triangulate);
	model_->AsyncLoad("resources/model/demo", "teapot.obj", Model::GetDefaultAssimpOption() | aiProcess_Triangulate);
	//model_->SetState(ExecutionState::kCompleted);

	camera_ = std::make_unique<Camera3d>();
	camera_->Create();
	camera_->GetTransform().translate = { 0.0f, 0.0f, -12.0f };
	camera_->UpdateMatrix();

	pipeline_ = std::make_unique<DxObject::ReflectionGraphicsPipelineState>();
	pipeline_->CreateBlob("demo/simple.vs.hlsl", DxObject::GraphicsShaderType::vs);
	pipeline_->CreateBlob("demo/simple.ps.hlsl", DxObject::GraphicsShaderType::ps);

	pipeline_->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

	DxObject::GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();
	desc.rtvFormats[0] = DxObject::kScreenFormat;

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

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

void GameScene::Update() {

	transform_.GetTransform().rotate *= MakeAxisAngle({0.0f, 1.0f, 0.0f}, 0.01f);
	transform_.UpdateMatrix();
}

void GameScene::Draw() {

	auto commandList = SxavengerSystem::GetCommandList();

	if (!subWindow_.expired()) {
		auto window = subWindow_.lock();

		window->BeginRendering();
		window->ClearWindow();
		window->EndRendering();
	}

	mainWindow_->BeginRendering();
	mainWindow_->ClearWindow();

	if (renderWindowSwitch_) {

		pipeline_->ReloadAndSetPipeline(commandList);

		if (model_->IsCompleted()) {

			DxObject::BindBufferDesc bind = {};
			bind.SetAddress("gTransform", transform_.GetGPUVirtualAddress());
			bind.SetAddress("gCamera", camera_->GetGPUVirtualAddress());

			for (uint32_t i = 0; i < model_->GetMeshSize(); ++i) {
				model_->SetIABuffer(i);

				bind.SetHandle("gDiffuse", model_->GetTextureHandle(i));
				pipeline_->BindGraphicsBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), bind);

				model_->DrawCall(i);
			}
		}
	}

	mainWindow_->EndRendering();
}

void GameScene::Term() {
	SxavengerSystem::ExecuteAllAllocator();
}


