#include "AssetSkeleton.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetSkeleton class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetSkeleton::Setup(const BornNode& root) {

	// skeletonの生成
	skeleton_.Create(root);

	BaseAsset::Complete();
	Logger::EngineThreadLog(std::format("[AssetSkeleton]: skeleton setup complete. uuid: {}", BaseAsset::GetId().Serialize()));
}
