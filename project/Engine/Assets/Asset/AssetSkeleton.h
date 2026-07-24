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
//! @brief ボーンノード階層からスケルトン(ジョイント構造)を構築し, 保持するAsset
class AssetSkeleton
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Skeleton class
	////////////////////////////////////////////////////////////////////////////////////////////

	//* constructor / destructor *//

	AssetSkeleton(const Uuid& id) : BaseAsset(id) {}

	~AssetSkeleton() override = default;

	//* setup option *//

	//! @brief ボーンノードの根からスケルトンを構築する
	//! @param[in] root ボーン階層のルートノード
	void Setup(const BornNode& root);

	//* getter *//

	//! @brief 構築済みのスケルトンを取得する
	//! @return スケルトンへの参照
	const Skeleton& GetSkeleton() const { return skeleton_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Skeleton skeleton_;

};

SXAVENGER_ENGINE_NAMESPACE_END
