#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/Asset/SxavengerAsset.h>

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

	//* parameter *//

	std::optional<std::filesystem::path> selectedDirectory_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowAssetWindow();

	//* helper *//

	void SetSelected(BaseAsset* asset);

	bool CheckSelectedInspector(BaseAsset* asset);

	//* show asset *//

	void ShowAssetDirectoryTable(const std::filesystem::path& path);
	void ShowAssetDirectoryNode();
	void ShowAssetLayout();

};
