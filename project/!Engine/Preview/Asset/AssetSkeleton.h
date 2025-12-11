#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "BaseAsset.h"

//* engine
#include <Engine/Content/Animation/BornNode.h>
#include <Engine/Content/Animation/Skeleton.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetSkeleton class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetSkeleton final
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetSkeleton(const Uuid& id) : BaseAsset(id) {}
	~AssetSkeleton() override = default;

	void Setup(const BornNode& root);

	//* getter *//

	const Skeleton& GetSkeleton() const { return skeleton_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Skeleton skeleton_;

};
