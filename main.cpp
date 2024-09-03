#define NOMINMAX
#include <Sxavenger.h>
#include <SxavengerGraphics.h>
#include <Environment.h>

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// GameScene
#include <GameScene.h>

// c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//=========================================================================================
	// 初期化
	//=========================================================================================
	Sxavenger::Init(kWindowWidth, kWindowHeight, kWindowTitle);
	SxavengerGraphics::Init();

	std::unique_ptr<GameScene> gameScene = std::make_unique<GameScene>();
	
	gameScene->Run();

	gameScene.reset();

	SxavengerGraphics::Term();
	Sxavenger::Term();
	return 0;
}