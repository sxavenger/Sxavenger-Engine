#define NOMINMAX
#include <Engine/System/Sxavenger.h>
//#include <SxavengerGraphics.h>
#include <Lib/Environment.h>

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// GameScene
#include <Engine/Game/GameScene.h>

// c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//=========================================================================================
	// 初期化
	//=========================================================================================
	Sxavenger::Init(kWindowSize, kWindowTitle);
	//SxavengerGraphics::Init();

	std::unique_ptr<GameScene> gameScene = std::make_unique<GameScene>();
	
	gameScene->Run();

	gameScene.reset();

	//SxavengerGraphics::Term();
	Sxavenger::Term();
	return 0;
}