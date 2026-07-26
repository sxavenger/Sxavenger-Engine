#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* execution
#include <Engine/System/Runtime/Execution/ExecutionInterface.h>

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXWindowContext.h>
#include <Engine/Module/Scene/SceneController.h>
#include <Engine/Module/Actor/PerformanceActor.h>

//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ExampleGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief SceneController(Stateパターン)でdemoシーンを駆動するゲームループの実装例
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

	//* scene *//

	std::unique_ptr<SceneController> sceneController_; //!< シーン(state)の保持と遷移を制御する

	//* persistent object *//

	std::unique_ptr<PerformanceActor> performance_; //!< シーンをまたいで常駐するパフォーマンス表示

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void RenderSystem();

};

SXAVENGER_ENGINE_NAMESPACE_END
