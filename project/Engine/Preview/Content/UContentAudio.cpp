#include "UContentAudio.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* uasset
#include "../Asset/UAssetStorage.h"
#include "../Asset/UAssetAudioClip.h"

//* engine
#include <Engine/System/Utility/ComPtr.h>

//* lib
#include <Lib/Adapter/Json/JsonHandler.h>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentAudio class methods
////////////////////////////////////////////////////////////////////////////////////////////

void UContentAudio::AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) {
	UBaseContent::CheckExist();

	Load(UBaseContent::GetFilepath());
}

void UContentAudio::AttachUuid() {
	UBaseContent::CheckExist();

	// idを取得
	GetUuid();

	// storageに登録
	auto asset = std::make_shared<UAssetAudioClip>(id_);
	sUAssetStorage->Register(asset, UBaseContent::GetFilepath());
}

void UContentAudio::Load(const std::filesystem::path& filepath) {

	ComPtr<IMFSourceReader> reader;
	auto hr = MFCreateSourceReaderFromURL(filepath.generic_wstring().c_str(), nullptr, &reader);

	DxObject::Assert(hr, L"IMFSourceReader create failed.");

	// assetの設定
	auto asset = sUAssetStorage->GetAsset<UAssetAudioClip>(id_);
	asset->Setup(reader.Get());
}

void UContentAudio::GetUuid() {
	std::filesystem::path filepath = UBaseContent::GetContentPath();

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
