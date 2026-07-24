#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* execution
#include <Engine/System/Runtime/Execution/ExecutionInterface.h>

//* engine
#include <Engine/Foundation.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// EngineExecution class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief エンジン本体(ランタイム)の実行処理を担うExecution Interface
class EngineExecution
	: public Execution::Interface {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(Execution::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* context methods *//

	void SetProcess(Execution::Context* context);

	//* loop helper methods *//

	void CreateWhite1x1();
	void CreateCheckerboard();

	void LoadContent();

	void UpdateAsset();

};

SXAVENGER_ENGINE_NAMESPACE_END
