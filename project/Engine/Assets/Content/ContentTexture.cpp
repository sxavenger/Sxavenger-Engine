#include "ContentTexture.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Asset/AssetStorage.h"
#include "../Asset/AssetTexture.h"

//* engine
#include <Engine/System/System.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentTexture::Attach(const std::filesystem::path& filepath, const std::any& parameter) {

	BaseContent::Attach(filepath, parameter);

	//!< Uuidの割り当て
	AttachUuid(filepath);

	//!< Storageに登録
	sAssetStorage->Register<AssetTexture>(id_, filepath);
}

void ContentTexture::Load(MAYBE_UNUSED const DirectXQueueContext* context) {

	DirectX::ScratchImage image = LoadContent(context, BaseContent::GetFilepath(), ContentTexture::GetOption());

	std::shared_ptr<AssetTexture> asset = sAssetStorage->Get<AssetTexture>(id_);
	asset->SetName(BaseContent::GetFilepath().filename().string());
	asset->Setup(context, image);

	BaseContent::SetComplete(); //!< 読み込み完了
}

std::string ContentTexture::GetEncoding(Encoding encoding) {
	switch (encoding) {
		case Encoding::Lightness: return "Lightness (sRGB)";
		case Encoding::Intensity: return "Intensity";

		default: return "Unknown";
	}
}

void ContentTexture::AttachUuid(const std::filesystem::path& filepath) {
	json meta = BaseContent::LoadMetaData(filepath);

	if (meta.contains("id")) {
		//!< idが既に存在する場合は、metaから取得する
		id_ = Uuid::Deserialize(meta["id"].get<std::string>());

	} else {
		//!< idが存在しない場合は、新しくidを生成し, metaに保存する
		id_ = Uuid::Generate();

		meta["id"] = id_.Serialize();
		BaseContent::SaveMetaData(meta, filepath);
	}
}

ContentTexture::Option ContentTexture::GetOption() {
	if (BaseContent::GetParameter().has_value()) {
		return std::any_cast<Option>(BaseContent::GetParameter());
	}

	return Option{}; //!< default option.
}

DirectX::ScratchImage ContentTexture::LoadContent(const DirectXQueueContext*, const std::filesystem::path& filepath, const Option& option) {
#if 0
	//!< Textureの圧縮処理
	if (option.isCompress && CheckCompress(filepath)) {
		StreamLogger::EngineThreadLog("[ContentTexture] compress texture. filepath: " + filepath.generic_string());
		Compress(filepath, option);
	}
#endif

	DirectX::ScratchImage image = {};

	if (ExistsCompressed(filepath)) {
		//!< compressの読み込み
		std::filesystem::path path = GetCompressedPath(filepath);
		image = LoadTexture(path, option);

	} else {
		//!< 通常imageの読み込み
		image = LoadTexture(filepath, option);
	}

	return image;
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
	if (option.encoding != ContentTexture::GetFormatEncoding(image.GetMetadata().format)) {
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
	if (option.encoding != ContentTexture::GetFormatEncoding(image.GetMetadata().format)) {
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
	if (option.encoding != ContentTexture::GetFormatEncoding(image.GetMetadata().format)) {
		StreamLogger::EngineThreadLog("[ContentTexture] warning | encoding is mismatched. filepath: " + filepath.generic_string());
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

	const std::filesystem::path& extension = filepath.extension();

	if (extension == ".dds" || extension == ".compress") { //!< filenameが".dds"または".compress"で終わっている場合
		return ContentTexture::LoadFromDDSFile(filepath, option);

	} else if (extension == ".hdr") { //!< filenameが".hdr"で終わっている場合
		return ContentTexture::LoadFromHDRFile(filepath, option);

	} else if (extension == ".tga") { //!< filenameが".tga"で終わっている場合
		return ContentTexture::LoadFromTGAFile(filepath, option);

	} else {
		return ContentTexture::LoadFromWICFile(filepath, option);
	}
}

bool ContentTexture::ExistsCompressed(const std::filesystem::path& filepath) const {
	return std::filesystem::exists(ContentTexture::GetCompressedPath(filepath));
}

bool ContentTexture::CheckCompress(const std::filesystem::path& filepath) const {
	//!< compressが必要か確認.
#ifdef _DEVELOPMENT

	LocalTimePoint fileTime = LocalTimePoint::Convert(
		std::chrono::clock_cast<std::chrono::system_clock>(std::filesystem::last_write_time(filepath))
	);

	json meta               = BaseContent::LoadMetaData(filepath);
	LocalTimePoint metaTime = {};

	if (meta.contains("compress")) {
		//!< compress時間がmetaに存在する場合は、metaから取得する
		metaTime = LocalTimePoint::Deserialize(meta["compress"].get<std::string>());

	} else {
		//!< compress時間がmetaに存在しない場合は、初回compressとみなす
		meta["compress"] = fileTime.Serialize();
		BaseContent::SaveMetaData(meta, filepath);
	}

	return !ExistsCompressed(filepath) //!< compressファイルが存在しない場合
		|| fileTime != metaTime;       //!< ファイルの更新日時とmetaのcompress日時が異なる場合
	//!< いずれかの条件でcompressが必要

#else
	return false;
#endif
}

std::filesystem::path ContentTexture::GetCompressedPath(const std::filesystem::path& filepath) {
	std::filesystem::path path = filepath;
	path += ".compress";
	return path;
}

void ContentTexture::Compress(const std::filesystem::path& filepath, const Option& option) {

	std::filesystem::path extension = filepath.extension();

	if (extension == ".dds" || extension == ".compress") {
		StreamLogger::EngineThreadLog("[ContentTexture] already compressed texture extension. filepath: " + filepath.generic_string());
		return; //!< dds, compressは既に圧縮されているので何もしない
	}

	DirectX::ScratchImage image = ContentTexture::LoadTexture(filepath, option);

	DXGI_FORMAT currentFormat = image.GetMetadata().format;

	if (DirectX::IsCompressed(currentFormat)) {
		StreamLogger::EngineThreadLog("[ContentTexture] already compressed format texture. filepath: " + filepath.generic_string());
		return; //!< 既に圧縮formatの場合は何もしない
	}

	// 圧縮フォーマットの選択:
	// LDR(通常の8bitカラー)はBC7が高品質かつRGBAを扱えるため既定とする. sRGBならsRGB版フォーマットへ変換する.
	// HDR(float)はBC7では表現できないため, HDR専用のBC6Hを使う (BC6HはsRGBの概念を持たないためMakeSRGBは適用しない).
	DXGI_FORMAT compressFormat = DXGI_FORMAT_BC7_UNORM;

	if (option.encoding == Encoding::Lightness) {
		//!< sRGB形式に変換
		compressFormat = DirectX::MakeSRGB(compressFormat);
	}

	if (currentFormat == DXGI_FORMAT_R32G32B32A32_FLOAT || currentFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) {
		//!< HDRの場合はBC6Hで圧縮
		compressFormat = DXGI_FORMAT_BC6H_UF16;
	}

	StreamLogger::EngineThreadLog(std::format(
		"[ContentTexture] compress texture target format: {} -> {}. filepath: {}",
		magic_enum::enum_name(currentFormat), magic_enum::enum_name(compressFormat), filepath.generic_string()
	));

	DirectX::ScratchImage compress = {};
	auto hr = DirectX::Compress(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		compressFormat,
		DirectX::TEX_COMPRESS_SRGB | DirectX::TEX_COMPRESS_BC7_QUICK,
		1.0f,
		compress
	);
	DxObject::Assert(hr, L"texture compress failed. filepath: " + filepath.generic_wstring());

	//!< 出力先を設定
	std::filesystem::path path = GetCompressedPath(filepath);

	//!< 圧縮したtextureをddsで保存
	hr = DirectX::SaveToDDSFile(
		compress.GetImages(),
		compress.GetImageCount(),
		compress.GetMetadata(),
		DirectX::DDS_FLAGS_NONE,
		path.generic_wstring().c_str()
	);
	DxObject::Assert(hr, L"compressed texture save failed. filepath: " + path.generic_wstring());

	StreamLogger::EngineThreadLog("[ContentTexture] compress texture complete. filepath: " + path.generic_string());
}
