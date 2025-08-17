#include "UContentTexture.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "../Asset/UAssetStorage.h"

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UContentTexture::AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) {
	UBaseContent::CheckExist();

	Option option = {}; //!< default option

	if (param_.has_value()) {
		option = std::any_cast<Option>(param_);
	}

	Load(context, UBaseContent::GetFilepath(), option);
}

void UContentTexture::Load(const DirectXQueueContext* context, const std::filesystem::path& filepath, const Option& option) {
	// idを取得
	GetUuid();

	// imageの読み込み
	DirectX::ScratchImage image = LoadTexture(filepath, option);

	// assetの生成
	auto asset = std::make_shared<UAssetTexture>(id_);
	sUAssetStorage->Register(asset, filepath);

	asset->Setup(context, image);
};

void UContentTexture::GetUuid() {
	//!< multi threadにする場合, thread safeにする必要がある.

	std::filesystem::path filepath = UBaseContent::GetContentPath();

	if (JsonHandler::CheckExist(filepath)) {
		//!< Idが既に存在する場合は、Json形式で読み込む
		json data = JsonHandler::LoadFromJson(filepath);
		id_ = Uuid::Deserialize(data["id"].get<std::string>());

	} else {
		//!< 新しくIdを生成し, Json形式で保存する
		id_ = Uuid::Generate();

		json data  = json::object();
		data["id"] = id_.Serialize();

		JsonHandler::WriteToJson(filepath, data);
	}
}

DirectX::ScratchImage UContentTexture::LoadFromDDSFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	// ddsファイルの読み込み
	auto hr = DirectX::LoadFromDDSFile(
		filepath.generic_wstring().c_str(),
		DirectX::DDS_FLAGS_NONE,
		nullptr,
		image
	);
	Exception::Assert(SUCCEEDED(hr), "texture load failed. filepath: " + filepath.generic_string());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		Logger::EngineThreadLog("[UContentTexture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
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

DirectX::ScratchImage UContentTexture::LoadFromHDRFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	// hdrファイルの読み込み
	auto hr = DirectX::LoadFromHDRFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		image
	);
	Exception::Assert(SUCCEEDED(hr), "texture load failed. filepath: " + filepath.generic_string());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		Logger::EngineThreadLog("[UContentTexture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
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
	Exception::Assert(SUCCEEDED(hr), "mipmaps create failed.");

	return mipimage;

}

DirectX::ScratchImage UContentTexture::LoadFromTGAFile(const std::filesystem::path& filepath, const Option& option) {

	DirectX::ScratchImage image = {};

	// tgaファイルの読み込み
	auto hr = DirectX::LoadFromTGAFile(
		filepath.generic_wstring().c_str(),
		nullptr,
		image
	);
	Exception::Assert(SUCCEEDED(hr), "texture load failed. filepath: " + filepath.generic_string());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		Logger::EngineThreadLog("[UContentTexture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
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
	Exception::Assert(SUCCEEDED(hr), "mipmaps create failed.");

	return mipimage;
}

DirectX::ScratchImage UContentTexture::LoadFromWICFile(const std::filesystem::path& filepath, const Option& option) {

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
	Exception::Assert(SUCCEEDED(hr), "texture load failed. filepath: " + filepath.generic_string());

	// encodingの設定と一致しているか確認
	if (option.encoding != GetFormatEncoding(image.GetMetadata().format)) {
		Logger::EngineThreadLog("[UContentTexture]: warning | encoding is mismatched. filepath: " + filepath.generic_string());
	}

	if (!option.isGenerateMipmap) {
		return image; //!< mipmapを生成しない場合はここで終了
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
	Exception::Assert(SUCCEEDED(hr), "mipmaps create failed.");

	return mipimage;

}

DirectX::ScratchImage UContentTexture::LoadTexture(const std::filesystem::path& filepath, const Option& option) {

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
