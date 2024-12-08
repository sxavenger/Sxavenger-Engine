#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/Performance/Performance.h>
#include <Engine/Console/Console.h>
#include <Engine/Content/SxavengerContent.h>
#include <Engine/Module/SxavengerModule.h>

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
		sConsole->Draw();
		DrawScreen();
		sConsole->DrawConsole();

		SxavengerSystem::PresentAllWindow();
		SxavengerSystem::ExecuteAllAllocator();
		//!< sub window delete時に死ぬ
		//!< shader hot reload 時に死ぬ

		SxavengerModule::ResetPrimtive();

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

void GameScene::SystemInit() {

	mainWindow_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, kMainWindowTitle).lock();
	mainWindow_->SetIcon("packages/icon/SxavengerEngineIcon.ico", { 32, 32 });

	sConsole->SetWindow(mainWindow_);

	subWindow_ = SxavengerSystem::TryCreateSubWindow(kMainWindowSize, L"sub").lock();

	{
		std::unique_ptr<DxObject::ReflectionComputePipelineState> compute = std::make_unique<DxObject::ReflectionComputePipelineState>();
		compute->CreateBlob("common/white1x1.cs.hlsl");
		compute->ReflectionPipeline(SxavengerSystem::GetDxDevice());

		std::shared_ptr<UnorderedTexture> white1x1 = SxavengerContent::TryCreateUnorderedTexture("white1x1", { 1, 1 });
		white1x1->TransitionBeginUnordered(SxavengerSystem::GetMainThreadContext());
		compute->SetPipeline(SxavengerSystem::GetCommandList());


		DxObject::BindBufferDesc bind = {};
		bind.SetHandle("gOutput", white1x1->GetGPUHandleUAV());
		compute->BindComputeBuffer(SxavengerSystem::GetMainThreadContext()->GetDxCommand(), bind);

		compute->Dispatch(SxavengerSystem::GetCommandList(), 1, 1, 1);

		white1x1->TransitionEndUnordered(SxavengerSystem::GetMainThreadContext());
		SxavengerSystem::TransitionAllocator();
	}
}

void GameScene::Init() {

	chess_ = std::make_unique<ChessBoard>();
	chess_->Init();
	chess_->SetToConsole();

	emitter_ = std::make_unique<Emitter>();
	emitter_->Init();
	emitter_->SetToConsole();

	smoke_ = std::make_unique<Smoke>();
	smoke_->Init();
	smoke_->SetToConsole();

	emitter_->SetParticle(smoke_.get());


	illmination_ = std::make_unique<Illmination>();
	illmination_->Init();
	illmination_->SetToConsole();
}

void GameScene::Update() {
	chess_->Update();
	emitter_->Update();
	smoke_->Update();
}

void GameScene::Draw() {
}

void GameScene::DrawScreen() {

	mainWindow_->BeginRendering();
	mainWindow_->ClearWindow();

	mainWindow_->EndRendering();

	
	if (!subWindow_.expired()) {
		auto window = subWindow_.lock();

		window->BeginRendering();
		window->ClearWindow();

		sConsole->PresentToScreen(window.get(), SxavengerSystem::GetMainThreadContext());

		window->EndRendering();
	}
}

void GameScene::Term() {
	chess_.reset();
	emitter_.reset();
	smoke_.reset();
	illmination_.reset();

	SxavengerSystem::ExecuteAllAllocator();
}


