#include "ContentTexture.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Asset/AssetStorage.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentTexture::AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) {
	BaseContent::CheckExist();

	Option option = GetOption();

	Load(context, BaseContent::GetFilepath(), option);
}

void ContentTexture::AttachUuid() {
	BaseContent::CheckExist();

	// idを取得
	AssignUuid();

	// storageに登録
	auto asset = std::make_shared<AssetTexture>(id_);
	sAssetStorage->Register(asset, BaseContent::GetFilepath());
}

void ContentTexture::ShowInspector() {
	BaseContent::ShowInspector();

	Option option = GetOption();

	if (ImGui::CollapsingHeader("Option", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Text("encoding: %s", GetEncoding(option.encoding).c_str());
	}

	if (ImGui::Button("Texture")) {
		BaseContent::SelectInspector(sAssetStorage->GetAsset<AssetTexture>(id_).get());
	}
}

void ContentTexture::Load(const DirectXQueueContext* context, const std::filesystem::path& filepath, const Option& option) {

#ifdef _DEVELOPMENT
	// TODO: Auto Compressを実行.
	//Compress(filepath, option);
#endif

	// imageの読み込み
	DirectX::ScratchImage image = LoadTexture(filepath, option);

	// assetの生成
	auto asset = sAssetStorage->GetAsset<AssetTexture>(id_);
	asset->Setup(context, image);
};

void ContentTexture::AssignUuid() {

	json meta = BaseContent::LoadMeta();

	if (meta.contains("id")) {
		//!< idが既に存在する場合は、metaから取得する
		id_ = Uuid::Deserialize(meta["id"].get<std::string>());

	} else {
		//!< idが存在しない場合は、新しくidを生成し, metaに保存する
		id_ = Uuid::Generate();

		meta["id"] = id_.Serialize();
		BaseContent::SaveMeta(meta);
	}
}

ContentTexture::Option ContentTexture::GetOption() {
	if (param_.has_value()) {
		return std::any_cast<Option>(param_);
	}

	return Option{}; //!< default option.
}

std::string ContentTexture::GetEncoding(Encoding encoding) {
	switch (encoding) {
		case Encoding::Lightness: return "Lightness (sRGB)";
		case Encoding::Intensity: return "Intensity";

		default: return "Unknown";
	}
}

DirectX::ScratchImage ContentTexture::LoadFromDDSFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	// ddsファイルの読み込み
	auto hr = DirectX::LoadFromDDSFile(
		filepath.generic_wstring().c_str(),
		DirectX::DDS_FLAGS_NONE,
		nullptr,
		image
	);
	DxObject::Assert(hr, L"texture load failed. filepath: " + filepath.generic_wstring());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		StreamLogger::EngineThreadLog("[ContentTexture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
	}

	if (DirectX::IsCompressed(image.GetMetadata().format)) { //!< 圧縮formatかどうか調べる
		return image;
	}

	if (!option.isGenerateMipmap) {
		return image; //!< mipmapを生成しない場合はここで終了
	}

	// mipmapの生成
	DirectX::ScratchImage mipimage = {};

	DirectX::TEX_FILTER_FLAGS flags = DirectX::TEX_FILTER_DEFAULT;

	if (option.encoding == Encoding::Lightness) {
		flags = DirectX::TEX_FILTER_SRGB;
	}

	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		flags,
		0,
		mipimage
	);

	return mipimage;
	
}

DirectX::ScratchImage ContentTexture::LoadFromHDRFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	// hdrファイルの読み込み
	auto hr = DirectX::LoadFromHDRFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		image
	);
	DxObject::Assert(hr, L"texture load failed. filepath: " + filepath.generic_wstring());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		StreamLogger::EngineThreadLog("[ContentTexture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
	}

	if (!option.isGenerateMipmap) {
		return image; //!< mipmapを生成しない場合はここで終了
	}

	// mipmapの生成
	DirectX::ScratchImage mipimage = {};

	DirectX::TEX_FILTER_FLAGS flags = DirectX::TEX_FILTER_DEFAULT;

	if (option.encoding == Encoding::Lightness) {
		flags = DirectX::TEX_FILTER_SRGB;
	}

	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		flags,
		0,
		mipimage
	);
	DxObject::Assert(hr, L"mipmaps create failed. filepath: " + filepath.generic_wstring());

	return mipimage;

}

DirectX::ScratchImage ContentTexture::LoadFromTGAFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	// tgaファイルの読み込み
	auto hr = DirectX::LoadFromTGAFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		image
	);
	DxObject::Assert(hr, L"texture load failed. filepath: " + filepath.generic_wstring());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		StreamLogger::EngineThreadLog("[ContentTexture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
	}

	if (!option.isGenerateMipmap) {
		return image; //!< mipmapを生成しない場合はここで終了
	}

	// mipmapの生成
	DirectX::ScratchImage mipimage = {};

	DirectX::TEX_FILTER_FLAGS flags = DirectX::TEX_FILTER_DEFAULT;

	if (option.encoding == Encoding::Lightness) {
		flags = DirectX::TEX_FILTER_SRGB;
	}

	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		flags,
		0,
		mipimage
	);
	DxObject::Assert(hr, L"mipmaps create failed. filepath: " + filepath.generic_wstring());

	return mipimage;
}

DirectX::ScratchImage ContentTexture::LoadFromWICFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	DirectX::WIC_FLAGS flags = DirectX::WIC_FLAGS_NONE;

	if (option.encoding == Encoding::Lightness) {
		flags = DirectX::WIC_FLAGS_FORCE_SRGB | DirectX::WIC_FLAGS_DEFAULT_SRGB;

	} else if (option.encoding == Encoding::Intensity) {
		flags = DirectX::WIC_FLAGS_FORCE_RGB;
	}

	// wicファイルの読み込み
	auto hr = DirectX::LoadFromWICFile(
		filepath.generic_wstring().c_str(),
		flags,
		nullptr,
		image
	);
	DxObject::Assert(hr, L"texture load failed. filepath: " + filepath.generic_wstring());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		StreamLogger::EngineThreadLog("[ContentTexture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
	}

	if (!option.isGenerateMipmap) {
		return image; //!< mipmapを生成しない場合はここで終了
	}

	if (image.GetMetadata().width == 1 && image.GetMetadata().height == 1) {
		return image; //!< 1x1の場合はmipmapを生成できないのでここで終了
	}

	// mipmapの生成
	DirectX::ScratchImage mipimage = {};

	DirectX::TEX_FILTER_FLAGS mipFlags = DirectX::TEX_FILTER_DEFAULT;

	if (option.encoding == Encoding::Lightness) {
		mipFlags = DirectX::TEX_FILTER_SRGB;
	}

	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		mipFlags,
		0,
		mipimage
	);
	DxObject::Assert(hr, L"mipmaps create failed. filepath: " + filepath.generic_wstring());

	return mipimage;

}

DirectX::ScratchImage ContentTexture::LoadTexture(const std::filesystem::path& filepath, const Option& option) {

	// TODO: CompressさせたTextureの読み込み.

	const std::filesystem::path& extension = filepath.extension();

	if (extension == ".dds") { //!< filenameが".dds"で終わっている場合
		return LoadFromDDSFile(filepath, option);

	} else if (extension == ".hdr") { //!< filenameが".hdr"で終わっている場合
		return LoadFromHDRFile(filepath, option);

	} else if (extension == ".tga") { //!< filenameが".tga"で終わっている場合
		return LoadFromTGAFile(filepath, option);

	} else {
		return LoadFromWICFile(filepath, option);
	}
}
