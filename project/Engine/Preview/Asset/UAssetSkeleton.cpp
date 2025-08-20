#include "UAssetSkeleton.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UAssetSkeleton class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UAssetSkeleton::Setup(const BornNode& root) {

	// skeletonの生成
	skeleton_.Create(root);

	UBaseAsset::Complete();
	Logger::EngineThreadLog(std::format("[UAssetSkeleton]: skeleton setup complete. uuid: {}", UBaseAsset::GetId().Serialize()));
}
