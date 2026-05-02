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

void ContentAudio::Attach(const std::filesystem::path& filepath, const std::any& parameter) {

	//!< 引数の保存
	BaseContent::Attach(filepath, parameter);

	//!< Uuidの割り当て
	AttachUuid(filepath);

	//!< Storageに登録
	sAssetStorage->Register<AssetAudioClip>(id_, filepath);

}

void ContentAudio::Load(MAYBE_UNUSED const DirectXQueueContext* context) {

	ComPtr<IMFSourceReader> reader;

	auto hr = MFCreateSourceReaderFromURL(BaseContent::GetFilepath().generic_wstring().c_str(), nullptr, &reader);
	DxObject::Assert(hr, L"IMFSourceReader create failed. filepath: " + BaseContent::GetFilepath().wstring());

#if 1 //!< 非同期Taskとして委任する場合.
	System::PushTask(
		Async::Execution::Cpu,
		std::format("AssetAudioClip {}", BaseContent::GetFilepath().filename().string()),
		[this, reader = std::move(reader)](const Async::ExecutionTask*, const DirectXQueueContext*) {
			std::shared_ptr<AssetAudioClip> asset = sAssetStorage->Get<AssetAudioClip>(id_);
			asset->Setup(reader.Get());
		}
	);
	
#else //!< 同一Taskとして実行する場合.
	std::shared_ptr<AssetAudioClip> asset = sAssetStorage->Get<AssetAudioClip>(id_);
	asset->Setup(reader.Get());
#endif

	BaseContent::SetComplete(); //!< 読み込み完了
}

void ContentAudio::AttachUuid(const std::filesystem::path& filepath) {

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
