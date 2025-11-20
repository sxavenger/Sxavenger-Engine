#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* editor
#include "BaseEditor.h"

//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/Preview/Content/BaseContent.h>
#include <Engine/Preview/Asset/AssetTexture.h>
#include <Engine/Preview/Asset/AssetParameter.h>

//* c++
#include <filesystem>

//////////////////////////////////////////////////////////////////////////////////////////
// AssetEditor class
//////////////////////////////////////////////////////////////////////////////////////////
//! @brief Asset関連Editorクラス
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

	AssetParameter<AssetTexture> folderTextures_;
	AssetParameter<AssetTexture> fileTexture_;

	std::unordered_map<const std::type_info*, AssetParameter<AssetTexture>> assetTextures_;

	//* parameter *//

	float iconSize_ = 64.0f;

	//* extension *//

	std::unordered_map<
		std::filesystem::path,
		std::pair<const std::type_info*, std::function<std::shared_ptr<BaseContent>(const std::filesystem::path&)>>
	> extensions_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* show imgui component *//

	void ShowAssetMenu();

	void ShowAssetWindow();

	//* imgui helper *//

	bool ImageButton(const std::filesystem::path& path, const AssetParameter<AssetTexture>& texture);

	//* helper *//

	static const std::string ConvertStr(const std::filesystem::path& path);

	void SetSelected(BaseContent* content);

	bool CheckSelectedInspector(BaseContent* content);

	void ForEachDirectory(const std::filesystem::path& path, const std::function<void(const std::filesystem::directory_entry&)>& func);

	static bool OpenShellExecuteApp(const std::filesystem::path& filepath);
	static bool OpenShellExecuteExplorer(const std::filesystem::path& filepath);

	template <ContentConcept T>
	void RegisterExtension(const std::filesystem::path& extension);

	//* show asset *//

	void ShowAssetDirectoryTable(const std::filesystem::path& path);
	void ShowAssetDirectoryNode();
	void ShowAssetLayout();

};

//////////////////////////////////////////////////////////////////////////////////////////
// AssetEditor class template methods
//////////////////////////////////////////////////////////////////////////////////////////

template <ContentConcept T>
inline void AssetEditor::RegisterExtension(const std::filesystem::path& extension) {
	extensions_.emplace(
		extension,
		std::make_pair(&typeid(T),
		[](const std::filesystem::path& filepath) -> std::shared_ptr<BaseContent> {
			std::shared_ptr<BaseContent> content = std::make_shared<T>();
			content->SetFilepath(filepath);

			return content;
		})
	);
}
