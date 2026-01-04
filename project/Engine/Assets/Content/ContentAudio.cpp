#include "ContentAudio.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Asset/AssetStorage.h"
#include "../Asset/AssetAudioClip.h"

//* engine
#include <Engine/System/Utility/ComPtr.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ContentAudio class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ContentAudio::AsyncLoad(MAYBE_UNUSED const DirectXQueueContext* context) {
	BaseContent::CheckExist();

	Load(BaseContent::GetFilepath());
}

void ContentAudio::AttachUuid() {
	BaseContent::CheckExist();

	// idを取得
	GetUuid();

	// storageに登録
	auto asset = std::make_shared<AssetAudioClip>(id_);
	sAssetStorage->Register(asset, BaseContent::GetFilepath());
}

void ContentAudio::Load(const std::filesystem::path& filepath) {

	ComPtr<IMFSourceReader> reader;
	auto hr = MFCreateSourceReaderFromURL(filepath.generic_wstring().c_str(), nullptr, &reader);

	DxObject::Assert(hr, L"IMFSourceReader create failed.");

	// assetの設定
	auto asset = sAssetStorage->GetAsset<AssetAudioClip>(id_);
	asset->Setup(reader.Get());
}

void ContentAudio::GetUuid() {
	std::filesystem::path filepath = BaseContent::GetContentPath();

	if (JsonHandler::CheckExist(filepath)) {
		//!< Idが既に存在する場合は、Json形式で読み込む
		json data = JsonHandler::LoadFromJson(filepath);
		id_ = Uuid::Deserialize(data["id"].get<std::string>());

	} else {
		//!< 新しくIdを生成し, Json形式で保存する
		id_ = Uuid::Generate();

		json data = json::object();
		data["id"] = id_.Serialize();

		JsonHandler::WriteToJson(filepath, data);
	}
}
