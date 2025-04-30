//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* gameloop
#include <Engine/GameLoop/SxavengerEngineGameLoop.h>
#include <Engine/GameLoop/EditorEngineGameLoop.h>
#include <Demo/GameLoop/BetaSystemGameLoop.h>
#include <Demo/GameLoop/DemoGameLoop.h>
#include <Demo/GameLoop/PreviewGameLoop.h>

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameLoop::Collection> collection = std::make_unique<GameLoop::Collection>();
	collection->Push<SxavengerEngineGameLoop>();
	collection->Push<EditorEngineGameLoop>();

	//collection->Push<BetaSystemGameLoop>();
	//collection->Push<DemoGameLoop>();
	collection->Push<PreviewGameLoop>();

	collection->Run();




	return 0;
}
