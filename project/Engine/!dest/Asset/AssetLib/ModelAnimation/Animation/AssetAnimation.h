#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../../BaseAsset.h"

//* engine
#include <Engine/System/Runtime/Thread/AsyncTask.h>
#include <Engine/Content/Animation/AnimationGroup.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetAnimation class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetAnimation
	: public BaseAsset, public AnimationGroup, public AsyncTask {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void SetAsyncTask(uint32_t assimpOption = NULL);

	void SetInspectorImGui() override;

	static bool CheckExtension(const std::filesystem::path& extension);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* load parameter *//

	uint32_t assimpOption_;

	//* extension *//

	static const std::unordered_set<std::filesystem::path> extension_;
};
