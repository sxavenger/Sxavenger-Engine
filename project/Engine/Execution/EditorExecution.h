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
// EditorExecution class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief エディタUIの実行処理を担うExecution Interface
class EditorExecution
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

	void InitEditor();

	void TermEditor();

	void UpdateEditor();

	void RenderEditor();

};

SXAVENGER_ENGINE_NAMESPACE_END
