//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Execution/ExecutionPipeline.h>

//* execution
#include <Engine/Execution/FeatureLevelExecution.h>
#include <Engine/Execution/EngineExecution.h>
#include <Engine/Execution/EditorExecution.h>
#include <Engine/Execution/ExampleGameLoop.h>
//#include <Demo/GameLoop/DemoGameLoop.h>

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<SxxEngine::Execution::Pipeline> pipeline = std::make_unique<SxxEngine::Execution::Pipeline>();
	pipeline->Push<SxxEngine::FeatureLevelExecution>(); //!< FeatureLevelの確認

	pipeline->Push<SxxEngine::EngineExecution>();

#ifdef _DEVELOPMENT
	pipeline->Push<SxxEngine::EditorExecution>();
#endif

	pipeline->Push<SxxEngine::ExampleGameLoop>();
	//collection->Push<DemoGameLoop>();

	pipeline->Run();

	return 0;
}
