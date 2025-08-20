#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/Preview/Content/UBaseContent.h>
#include <Engine/Preview/Asset/UAssetTexture.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

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

	UAssetParameter<UAssetTexture> folderTextures_;
	UAssetParameter<UAssetTexture> fileTexture_;

	std::unordered_map<const std::type_info*, UAssetParameter<UAssetTexture>> assetTextures_;

	//* parameter *//

	float iconSize_ = 64.0f;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowAssetWindow();

	//* imgui helper *//

	bool ImageButton(const std::filesystem::path& path, const UAssetParameter<UAssetTexture>& texture);

	//* helper *//

	static const std::string ConvertStr(const std::filesystem::path& path);

	void SetSelected(UBaseContent* content);

	bool CheckSelectedInspector(UBaseContent* content);

	void ForEachDirectory(const std::filesystem::path& path, const std::function<void(const std::filesystem::directory_entry&)>& func);

	static bool OpenShellExecuteApp(const std::filesystem::path& filepath);
	static bool OpenShellExecuteExplorer(const std::filesystem::path& filepath);

	//* show asset *//

	void ShowAssetDirectoryTable(const std::filesystem::path& path);
	void ShowAssetDirectoryNode();
	void ShowAssetLayout();

};
