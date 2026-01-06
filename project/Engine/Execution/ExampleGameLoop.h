#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* execution
#include <Engine/System/Runtime/Execution/ExecutionInterface.h>

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/Module/GameObject/GameObject.h>
#include <Engine/Module/Actor/ControllableCameraActor.h>
#include <Engine/Module/Actor/PerformanceActor.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ExampleGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class ExampleGameLoop
	: public Execution::Interface {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	void Init(Execution::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* window *//

	std::shared_ptr<DirectXWindowContext> main_;

	//* game object *//

	std::unique_ptr<GameObject> atmosphere_;
	std::unique_ptr<ControllableCameraActor> camera_;

	std::unique_ptr<PerformanceActor> performance_;

	std::unique_ptr<GameObject> demoText_;
	std::unique_ptr<GameObject> instructionText_;

	//* test *//

	std::unique_ptr<GameObject> test_;


	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void RenderSystem();

};

SXAVENGER_ENGINE_NAMESPACE_END
