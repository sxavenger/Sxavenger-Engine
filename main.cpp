//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Game/SxavengerGame.h>
#include <Engine/Game/GameScene.h>
#include <Engine/Console/SystemConsole.h>

//* lib
#include <Lib/Environment.h>

// c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Sxavenger::Init(kWindowSize, kWindowTitle);
	SxavengerGame::Init();

	sSystemConsole->Init();

	std::unique_ptr<GameScene> gameScene = std::make_unique<GameScene>();
	gameScene->Run();

	gameScene.reset();

	sSystemConsole->Term();

	SxavengerGame::Term();
	Sxavenger::Term();
	return 0;
}