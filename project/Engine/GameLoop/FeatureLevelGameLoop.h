#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/GameLoop/GameLoop.h>
#include <Engine/System/DirectX/DxObject/DxObjectCommon.h>

//* c++

////////////////////////////////////////////////////////////////////////////////////////////
// FeatureLevelGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class FeatureLevelGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================


	//* file *//

	static inline const std::filesystem::path filepath_ = "FeatureLevel.md";
	std::ofstream file_;

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
