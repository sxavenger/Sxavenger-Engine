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

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<SxxEngine::Execution::Pipeline> pipeline = std::make_unique<SxxEngine::Execution::Pipeline>();
	pipeline->Push<SxxEngine::EngineExecution>();

#ifdef _DEVELOPMENT
	pipeline->Push<SxxEngine::EditorExecution>();
#endif

	pipeline->Push<SxxEngine::ExampleGameLoop>();

	pipeline->Run();

	return 0;
}
