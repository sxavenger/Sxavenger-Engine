#include "GameScene.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

//* lib
#include <Lib/Environment.h>

#include "Engine/System/DirectX/DxObject/DxDimensionBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// GameScene class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameScene::Run() {

	//-----------------------------------------------------------------------------------------
	// 初期化処理
	//-----------------------------------------------------------------------------------------
	Init();

	//-----------------------------------------------------------------------------------------
	// ゲームループ
	//-----------------------------------------------------------------------------------------
	while (SxavengerSystem::ProcessMessage()) {

		//-----------------------------------------------------------------------------------------
		// 更新処理
		//-----------------------------------------------------------------------------------------

		Update();

		//-----------------------------------------------------------------------------------------
		// 描画処理
		//-----------------------------------------------------------------------------------------

		Draw();

		/*if (false) {
			break;
		}*/
	}

	//-----------------------------------------------------------------------------------------
	// 終了処理
	//-----------------------------------------------------------------------------------------

	Term();

}

void GameScene::Init() {
	mainWindow_ = SxavengerSystem::CreateMainWindow(kMainWindowSize, kMainWindowTitle);
	mainWindow_->SetIcon("resources/icon/SxavengerEngineIcon.ico", { 32, 32 });

	std::unique_ptr<DxObject::DimensionBuffer<float>> dimension = std::make_unique< DxObject::DimensionBuffer<float>>();
	dimension->Create(SxavengerSystem::GetDxDevice(), 2);

	(*dimension)[0] = (*dimension)[1];
	(*dimension)[2];
}

void GameScene::Update() {
	SxavengerSystem::TransitionAllocator();
}

void GameScene::Draw() {

	mainWindow_->BeginRendering();
	mainWindow_->ClearWindow();

	mainWindow_->EndRendering();

	SxavengerSystem::PresentAllWindow();
}

void GameScene::Term() {
	SxavengerSystem::ExecuteAllAllocator();
}


