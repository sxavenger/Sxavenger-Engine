#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"
#include "Animation/AssetAnimation.h"
#include "Model/AssetModel.h"

////////////////////////////////////////////////////////////////////////////////////////////
// AssetAnimation class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetModelAnimation
	: public BaseAsset {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	std::shared_ptr<AssetModel> CreateModel();
	std::shared_ptr<AssetModel> TryCreateModel();

	std::shared_ptr<AssetAnimation> CreateAnimation();
	std::shared_ptr<AssetAnimation> TryCreateAnimation();

	void SetInspectorImGui() override;

	static bool CheckExtension(const std::filesystem::path& extension);

	//* getter *//

	std::shared_ptr<AssetModel> GetModel() const { return modelAnimation_.first; }
	std::shared_ptr<AssetAnimation> GetAnimation() const { return modelAnimation_.second; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::pair<std::shared_ptr<AssetModel>, std::shared_ptr<AssetAnimation>> modelAnimation_;

};
