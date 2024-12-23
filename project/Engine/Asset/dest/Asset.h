#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "AssetLibrary/Texture/AssetTexture.h"
#include "AssetLibrary/Model/AssetModel.h"
#include "AssetLibrary/Unkown/AssetUnkown.h"

//* engine
#include <Engine/System/UI/ISystemDebugGui.h>

//* lib
#include <Lib/Sxl/OptimizedMap.h>

//* c++
#include <variant>
#include <memory>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Asset class
////////////////////////////////////////////////////////////////////////////////////////////
class Asset
	: public ISystemDebugGui {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////
	using Files = std::variant<
		std::shared_ptr<AssetTexture>,
		std::shared_ptr<AssetModel>,
		std::shared_ptr<AssetUnkown>
	>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Node structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Node {
		Sxl::OptimizerdMap<std::filesystem::path, Node>  folders;
		Sxl::OptimizerdMap<std::filesystem::path, Files> files;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* import methods *//

	Files Import(const std::filesystem::path& filepath);

	std::shared_ptr<AssetModel> ImportModel(const std::filesystem::path& filepath);
	std::shared_ptr<AssetTexture> ImportTexture(const std::filesystem::path& filepath);

	//* getter *//

	Files GetFile(const std::filesystem::path& filepath) const;

	std::shared_ptr<AssetModel> GetModel(const std::filesystem::path& filepath) const;
	std::shared_ptr<AssetTexture> GetTexture(const std::filesystem::path& filepath) const;

	//* override *//

	void SystemDebugGui() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Node root_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	Files ImportFile(const std::filesystem::path& filepath);

	void SystemDebugGuiNode(const Node& node);

};