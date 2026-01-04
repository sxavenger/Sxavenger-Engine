#include "AssetSkeleton.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/StreamLogger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetSkeleton class methods
////////////////////////////////////////////////////////////////////////////////////////////

void AssetSkeleton::Setup(const BornNode& root) {

	// skeletonの生成
	skeleton_.Create(root);

	BaseAsset::Complete();
	StreamLogger::EngineThreadLog(std::format("[AssetSkeleton]: skeleton setup complete. uuid: {}", BaseAsset::GetId().Serialize()));
}
