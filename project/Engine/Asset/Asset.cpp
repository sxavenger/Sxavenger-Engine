#include "Asset.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Asset class methods
////////////////////////////////////////////////////////////////////////////////////////////

Asset::Files Asset::Import(const std::filesystem::path& filepath) {

	Node* node = &root_;

	for (const auto& path : filepath) {
		if (path.has_extension()) {
			return node->files[path] = ImportFile(filepath);

		} else {
			node = &node->folders[path];
		}
	}

	return {};
}

std::shared_ptr<AssetModel> Asset::ImportModel(const std::filesystem::path& filepath) {
	Files data = Import(filepath);

	Assert(std::holds_alternative<std::shared_ptr<AssetModel>>(data), "import file is not model.");
	return std::get<std::shared_ptr<AssetModel>>(data);
}

std::shared_ptr<AssetTexture> Asset::ImportTexture(const std::filesystem::path& filepath) {
	Files data = Import(filepath);

	Assert(std::holds_alternative<std::shared_ptr<AssetTexture>>(data), "import file is not texture.");
	return std::get<std::shared_ptr<AssetTexture>>(data);
}

Asset::Files Asset::GetFile(const std::filesystem::path& filepath) const {

	const Node* node = &root_;

	for (const auto& path : filepath) {
		if (path.has_extension()) {
			return node->files.At(path);

		} else {
			node = &node->folders.At(path);
		}
	}

	return {};
}

std::shared_ptr<AssetModel> Asset::GetModel(const std::filesystem::path& filepath) const {
	Files file = GetFile(filepath);

	Assert(std::holds_alternative<std::shared_ptr<AssetModel>>(file), "type is not match.");
	return std::get<std::shared_ptr<AssetModel>>(file);
}

std::shared_ptr<AssetTexture> Asset::GetTexture(const std::filesystem::path& filepath) const {
	Files file = GetFile(filepath);

	Assert(std::holds_alternative<std::shared_ptr<AssetTexture>>(file), "type is not match.");
	return std::get<std::shared_ptr<AssetTexture>>(file);
}

void Asset::SystemDebugGui() {
	SystemDebugGuiNode(root_);
}

Asset::Files Asset::ImportFile(const std::filesystem::path& filepath) {

	if (AssetTexture::CheckExtension(filepath.extension())) {
		std::shared_ptr<AssetTexture> texture = std::make_shared<AssetTexture>();
		texture->SetFilepath(filepath);
		return texture;

	} else if (AssetModel::CheckExtension(filepath.extension())) {
		std::shared_ptr<AssetModel> model = std::make_shared<AssetModel>();
		model->SetFilepath(filepath);
		return model;
	}

	std::shared_ptr<AssetUnkown> unkown = std::make_shared<AssetUnkown>();
	unkown->SetFilepath(filepath);
	return unkown;
}

void Asset::SystemDebugGuiNode(const Node& node) {
	for (const auto& folder : node.folders.GetMap()) {
		if (ImGui::TreeNode(folder.first.generic_string().c_str())) {
			SystemDebugGuiNode(folder.second);
			ImGui::TreePop();
		}
	}

	for (const auto& file : node.files.GetMap()) {
		ImGui::Text(file.first.generic_string().c_str());
	}
}
