//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Window/GameWindow.h>

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

	std::unique_ptr<GameWindowCollection> collection = std::make_unique<GameWindowCollection>();
	collection->CreateMainWindow({1280, 720}, L"main");
	collection->TryCreateSubWindow({ 1280, 720 }, L"sub");

	while (collection->ProcessMessages()) {
	}

	return 0;
}