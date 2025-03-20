#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"
#include "Animator.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Asset Animator class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetAnimator
	: public BaseAsset, public Animator {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetAnimator()  = default;
	~AssetAnimator() = default;

	void Load(_MAYBE_UNUSED const DirectXThreadContext* context) override;

	//* setter *//

	void SetAssimpOption(uint32_t option) { assimpOption_ = option; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* input parameter *//

	uint32_t assimpOption_ = 0;

};
