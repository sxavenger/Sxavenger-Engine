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

	//* extension *//

	std::unordered_map<
		std::filesystem::path,
		std::pair<const std::type_info*, std::function<std::shared_ptr<UBaseContent>(const std::filesystem::path&)>>
	> extensions_;

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

	template <UContentConcept T>
	void RegisterExtension(const std::filesystem::path& extension);

	//* show asset *//

	void ShowAssetDirectoryTable(const std::filesystem::path& path);
	void ShowAssetDirectoryNode();
	void ShowAssetLayout();

};

//////////////////////////////////////////////////////////////////////////////////////////
// AssetEditor class template methods
//////////////////////////////////////////////////////////////////////////////////////////

template <UContentConcept T>
inline void AssetEditor::RegisterExtension(const std::filesystem::path& extension) {
	extensions_.emplace(
		extension,
		std::make_pair(&typeid(T),
		[](const std::filesystem::path& filepath) -> std::shared_ptr<UBaseContent> {
			std::shared_ptr<UBaseContent> content = std::make_shared<T>();
			content->SetFilepath(filepath);

			return content;
		})
	);
}
