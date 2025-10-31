//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* gameloop
#include <Engine/GameLoop/FeatureLevelGameLoop.h>
#include <Engine/GameLoop/SxavengerEngineGameLoop.h>
#include <Engine/GameLoop/EditorEngineGameLoop.h>
#include <Engine/GameLoop/BetaSystemGameLoop.h>
#include <Demo/GameLoop/DemoGameLoop.h>
#include <Demo/GameLoop/PreviewGameLoop.h>

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<GameLoop::Collection> collection = std::make_unique<GameLoop::Collection>();
	//collection->Push<FeatureLevelGameLoop>();
	
	collection->Push<SxavengerEngineGameLoop>();

#ifdef _DEVELOPMENT
	collection->Push<EditorEngineGameLoop>();
#endif
	
	//collection->Push<BetaSystemGameLoop>();
	//collection->Push<PreviewGameLoop>();

	collection->Push<DemoGameLoop>();

	collection->Run();

	return 0;
}
