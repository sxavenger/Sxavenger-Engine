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

//* demo
#include <Demo/Player/Player.h>
#include <Demo/Player/PlayerHandle.h>
#include <Demo/Camera/PerspectiveCamera.h>
#include <Demo/Camera/CameraHandle.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class DemoGameLoop
	: public SxxEngine::Execution::Interface {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	void Init(SxxEngine::Execution::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* window *//

	std::shared_ptr<SxxEngine::DirectXWindowContext> main_;

	//* gameobject *//

	std::unique_ptr<SxxEngine::GameObject> atmosphere_;

	std::unique_ptr<Player>       player_;
	std::unique_ptr<PlayerHandle> playerHandle_;

	std::unique_ptr<PerspectiveCamera> camera_;
	std::unique_ptr<CameraHandle>      cameraHandle_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void RenderSystem();

};
