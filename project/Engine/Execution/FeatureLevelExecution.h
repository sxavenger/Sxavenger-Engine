#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* execution
#include <Engine/System/Runtime/Execution/ExecutionInterface.h>

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// FeatureLevelExecution class
////////////////////////////////////////////////////////////////////////////////////////////
class FeatureLevelExecution
	: public Execution::Interface {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(Execution::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* DirectX12 interfaces *//

	ComPtr<IDXGIFactory7> factory_;
	ComPtr<IDXGIAdapter4> adapter_;
	ComPtr<ID3D12Device8> device_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* support methods *//



	//* main methods *//
	//!< DirectX12 feature level check
	void CheckFeatureLevel();

	bool CheckFactory();
	bool CheckAdapter();
	bool CheckDevice();

	void CheckSupport();
};

SXAVENGER_ENGINE_NAMESPACE_END
