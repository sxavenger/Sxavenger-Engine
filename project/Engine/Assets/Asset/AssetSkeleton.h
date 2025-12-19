#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/Graphics/Animation/BornNode.h>
#include <Engine/Graphics/Animation/Skeleton.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

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

SXAVENGER_ENGINE_NAMESPACE_END
