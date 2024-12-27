//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* loops
#include <Engine/GameLoop/EngineGameLoop.h>
#include <Engine/GameLoop/ConsoleGameLoop.h>
#include <Demo/GameLoop/DemoGameLoop.h>

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameLoop::Collection> collection = std::make_unique<GameLoop::Collection>();
	collection->Push<EngineGameLoop>();
	collection->Push<ConsoleGameLoop>();
	collection->Push<DemoGameLoop>();

	collection->Run();

	return 0;
}
