#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../IAsset.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/System/Runtime/Thread/Thread.h>
#include <Engine/Content/Model/Model.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// AssetModel class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetModel
	: public IAsset, public Model, public TaskThreadExecution {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void AsyncLoad(uint32_t assimpOption);

	void SetFilepath(const std::filesystem::path& filepath) { filepath_ = filepath; }

	void SetInspectorImGui() override;

	static bool CheckExtension(const std::filesystem::path& extension);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::filesystem::path filepath_;
	uint32_t              assimpOption_;

	//* extension *//

	static const std::unordered_set<std::filesystem::path> extension_;

};