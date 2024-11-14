//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Window/Window.h>
#include <Engine/System/Thread/Thread.h>

//* lib
#include <Lib/Environment.h>

// c++
#include <memory>

#pragma comment(lib, "dxgi.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	/*Sxavenger::Init(kWindowSize, kWindowTitle);
	SxavengerGame::Init();

	sSystemConsole->Init();

	std::unique_ptr<GameScene> gameScene = std::make_unique<GameScene>();
	gameScene->Run();

	gameScene.reset();

	sSystemConsole->Term();

	SxavengerGame::Term();
	Sxavenger::Term();*/

	std::unique_ptr<ThreadCollection> collection = std::make_unique<ThreadCollection>();
	collection->Init(4);

	return 0;
}