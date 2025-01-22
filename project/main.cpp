//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* loops
#include <Engine/GameLoop/EngineGameLoop.h>
#include <Engine/GameLoop/EditorEngineGameLoop.h>
#include <Engine/GameLoop/BetaSystemGameLoop.h>
#include <Game/GameLoop/ActionGameLoop.h>

////////////////////////////////////////////////////////////////////////////////////////////
// メイン関数
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameLoop::Collection> collection = std::make_unique<GameLoop::Collection>();
	collection->Push<EngineGameLoop>();
	collection->Push<EditorEngineGameLoop>();
	//collection->Push<BetaSystemGameLoop>();
	collection->Push<ActionGameLoop>();

	collection->Run();

	return 0;
}
