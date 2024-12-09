#include "SxavengerAsset.h"

////////////////////////////////////////////////////////////////////////////////////////////
// namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace {
	static std::unique_ptr<Asset> sAsset = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerAsset class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SxavengerAsset::Init() {
	sAsset = std::make_unique<Asset>();
}

void SxavengerAsset::Term() {
	sAsset.reset();
}

Asset::Files SxavengerAsset::Import(const std::filesystem::path& filepath) {
	return sAsset->Import(filepath);
}

std::shared_ptr<AssetTexture> SxavengerAsset::ImportTexture(const std::filesystem::path& filepath) {
	return sAsset->ImportTexture(filepath);
}

std::shared_ptr<AssetModel> SxavengerAsset::ImportModel(const std::filesystem::path& filepath) {
	return sAsset->ImportModel(filepath);
}

Asset* SxavengerAsset::GetAsset() {
	return sAsset.get();
}
