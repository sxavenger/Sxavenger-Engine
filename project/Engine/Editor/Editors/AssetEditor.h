#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/Asset/SxavengerAsset.h>

//* c++
#include <filesystem>

//////////////////////////////////////////////////////////////////////////////////////////
// AssetEditor class
//////////////////////////////////////////////////////////////////////////////////////////
class AssetEditor final
	: public BaseEditor {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	AssetEditor(EditorEngine* editor) : BaseEditor(editor) {}
	~AssetEditor() = default;

	void Init() override;

	void ShowMainMenu() override;

	void ShowWindow() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<std::filesystem::path> selectedDirectory_;

	//* textures *//

	AssetObserver<AssetTexture> folderTextures_;
	AssetObserver<AssetTexture> objectTextures_;

	//* parameter *//

	float iconSize_ = 64.0f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowAssetWindow();

	//* helper *//

	static const std::string ConvertStr(const std::filesystem::path& path);

	void SetSelected(BaseAsset* asset);

	bool CheckSelectedInspector(BaseAsset* asset);

	void ForEachDirectory(const std::filesystem::path& path, const std::function<void(const std::filesystem::directory_entry&)>& func);

	//* show asset *//

	void ShowAssetDirectoryTable(const std::filesystem::path& path);
	void ShowAssetDirectoryNode();
	void ShowAssetLayout();

};
