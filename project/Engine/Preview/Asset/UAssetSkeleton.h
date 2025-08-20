#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "UBaseAsset.h"

//* engine
#include <Engine/Content/Animation/BornNode.h>
#include <Engine/Content/Animation/Skeleton.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetSkeleton class
////////////////////////////////////////////////////////////////////////////////////////////
class UAssetSkeleton final
	: public UBaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UAssetSkeleton(const Uuid& id) : UBaseAsset(id) {}
	~UAssetSkeleton() override = default;

	void Setup(const BornNode& root);

	//* getter *//

	const Skeleton& GetSkeleton() const { return skeleton_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Skeleton skeleton_;

};
