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

	void Load(_MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const override { return AsyncExecution::None; }

private:
};
