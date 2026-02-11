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
	AssignUuid();

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

void ContentAudio::AssignUuid() {

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
