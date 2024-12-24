#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../../BaseAsset.h"

//* engine
#include <Engine/System/Runtime/Thread/AsyncTask.h>
#include <Engine/Content/Model/Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetModel class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetModel
	: public BaseAsset, public Model, public AsyncTask {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void SetAsyncTask(uint32_t assimpOption = Model::GetDefaultAssimpOption());

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
